/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

.global z_thread_switch
.global z_thread_entry
.global z_yield

.extern z_ker
.extern z_sched_setup_context_switch			; struct k_thread *(void)
.extern k_abort				; void (struct k_thread *)
.extern __fault				; void (uint8_t)

z_yield:
	/*
	 * call scheduler to determine which thread is next
	 */
    call z_sched_setup_context_switch

    movw r26, r24 ; r24 contains previous thread
	
    lds r30, z_ker ; Z contains current thread
    lds r31, z_ker + 1

#if CONFIG_KERNEL_SCHEDULER_COMPARE_THREADS_BEFORE_SWITCH
	/* 
	 * Compare thread addresses after the scheduler call to prevent a thread 
	 * switch to the same thread. 
	 * 
	 * i.e. Thread switch should only occur if the "previous" and "z_ker.current" 
	 * threads are different. 
	 * 
	 * This approach may be inefficient if at least two threads are always 
	 * ready. 
	 * 
	 * The comparison starts with the low bytes before the high bytes because 
	 * they have a higher probability of being different.
	 */
    cp r26, r30
    brne z_thread_switch
    cp r27, r31
    brne z_thread_switch
    ret
#endif


; "from" thread addr is in r24, r25
; "to" thread  addr in in r22, r23
z_thread_switch:
    push r2
    push r3
    push r4
    push r5
    push r6
    push r7
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push r16
    push r17
    push r28
    push r29
    lds r17, SREG
    push r17
	
	; save context and write SP in thread structure
    lds r20, SPL
    lds r21, SPH
    st X+, r20
    st X+, r21

	; restore SP and restore context
    ld r20, Z+
    ld r21, Z+
    sts SPL, r20
    sts SPH, r21

    pop r17
    sts SREG, r17
    pop r29
    pop r28
    pop r17

#if CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
    ldi     r16, CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
    sts     PINB, r16
#endif /* CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG */

    pop r16
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop r7
    pop r6
    pop r5
    pop r4
    pop r3
    pop r2

    ret

z_thread_entry:
	; get thread context address
    movw r24, r2

	; get thread_entry function address
    movw r30, r4

	; write SREG flags for the thread
    sts SREG, r6
	
#if CONFIG_KERNEL_THREAD_TERMINATION_TYPE == 0
	; jump to thread entry function
    ijmp
#elif CONFIG_KERNEL_THREAD_TERMINATION_TYPE == -1
	; call thread entry function
    icall

	/*
	 * Exiting the thread will cause a fault
	 */
    ldi r24, 0x04 ; K_FAULT_THREAD_TERMINATED
    call __fault
#else
	; call thread entry function
    icall

	/*
	 * Graceful/permitted termination of the thread
	 */
    call k_abort
#endif

#if CONFIG_KERNEL_DEBUG_PREEMPT_UART
.global USART0_RX_vect
USART0_RX_vect:
	/*
	* This debug interrupt handler is used to trigger a thread switch when a 
	* byte is received on the UART.
	*/
    push    r1
    push    r0
    lds    r0, SREG
    push    r0
    eor    r1, r1
    push    r18
    push    r19
    push    r20
    push    r21
    push    r22
    push    r23
    push    r24
    push    r25
    push    r26
    push    r27
    push    r30
    push    r31

    lds r24, UDR0
    call serial_transmit

    ldi r24, pm_lo8(__intctx_restore)
    ldi r25, pm_hi8(__intctx_restore)
    push r24
    push r25
#if defined(__AVR_3_BYTE_PC__)
    push r1
#endif 

    jmp z_yield
#endif
; CONFIG_KERNEL_DEBUG_PREEMPT_UART