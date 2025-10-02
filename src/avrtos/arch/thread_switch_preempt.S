/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

/*
 * Select the appropriate timer interrupt vector based on the
 * configuration.
 */
#if CONFIG_KERNEL_SYSLOCK_HW_TIMER == 0
#define TIMERn_COMPA_vect TIMER0_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 1
#define TIMERn_COMPA_vect TIMER1_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 2
#define TIMERn_COMPA_vect TIMER2_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 3
#define TIMERn_COMPA_vect TIMER3_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 4
#define TIMERn_COMPA_vect TIMER4_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 5
#define TIMERn_COMPA_vect TIMER5_COMPA_vect
#endif

.global z_thread_switch
.global z_thread_entry
.global z_yield
.global TIMERn_COMPA_vect

.extern z_ker
.extern z_scheduler			; struct k_thread *(void)
.extern z_sched_enter		; void (void)
.extern k_abort				; void (struct k_thread *)
.extern __fault				; void (uint8_t)

/*
 * This is our system tick interrupt handler, which is called at
 * the period specified by CONFIG_KERNEL_SYSCLOCK_PERIOD_US
 */
TIMERn_COMPA_vect:
	/*
	 * Save the registers that we are going to use for the increment
	 * of the system tick counter.
	 */
    push    r1
    push    r0
    lds    r0, SREG
    push    r0
    eor    r1, r1
    push    r18

#if CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
	/*
	 * Toggle the GPIO B pin specified by CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
	 * on each system tick. 
	 */
    ldi     r18, CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
    sts     PINB, r18
#endif

#if CONFIG_KERNEL_TICKS_COUNTER
	/*
	 * Increment the system tick counter.
	 */
    ldi r18, 0x01			; 1 cycle

    lds r0, z_ker + 2			; 2 cycles
    add r0, r18			; 1 cycle
    sts z_ker + 2, r0			; 2 cycles

    lds r0, z_ker + 3		; 2 cycles
    adc r0, r1			; 1 cycle
    sts z_ker + 3, r0		; 2 cycles
	
    lds r0, z_ker + 4		; 2 cycles
    adc r0, r1			; 1 cycle
    sts z_ker + 4, r0		; 2 cycles

    lds r0, z_ker + 5		; 2 cycles
    adc r0, r1			; 1 cycle
    sts z_ker + 5, r0		; 2 cycles

#if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
	/*
	 * In case we have a 40-bit counter, increment the upper 32:40 bits
	 * of the counter.
	 */
    lds r0, z_ker + 6		; 2 cycles
    adc r0, r1			; 1 cycle
    sts z_ker + 6, r0		; 2 cycles
#endif
#endif

#if Z_KERNEL_TIME_SLICE_MULTIPLE_TICKS
	/*
	 * If the time slice is doesn't match the system tick period,
	 * (i.e. is a multiple of the system tick period), decrement
	 * the number of ticks remaining for the current thread.
	 */
    lds r18, z_ker + 2 + CONFIG_KERNEL_TICKS_COUNTER_SIZE
    subi r18, 0x01

	/*
	 * If the number of ticks remaining is zero, we need to
	 * call the scheduler to evaluate a possible thread switch.
	 * 
	 * Otherwise, we just exit the interrupt.
	 */
    sts z_ker + 2 + CONFIG_KERNEL_TICKS_COUNTER_SIZE, r18
    brne __intctx_restore_minimal
#endif 

	/*
	 * Save the rest of the registers
	 */
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

#if CONFIG_KERNEL_SYSCLOCK_DEBUG
	/*
	 * Write a dot '.' to the serial port to indicate that a
	 * call to the scheduler is being made.
	 */
    ldi r24, 0x2e           ; '.'
    call serial_transmit
#endif

	/*
	 * Call scheduler to evaluate timeouts for threads/timers and events.
	 * Schedule threads consequently. 
	 */
    call z_sched_enter

#if CONFIG_KERNEL_COOPERATIVE_THREADS
	/* 
	 * Determine if the current thread is eligible for preemption, 
	 * meaning it is neither a cooperative thread nor the scheduler is locked.
	 */
    lds ZL, z_ker       ; load current thread addr in Z
    lds ZH, z_ker + 1
    ldd r18, Z + 2      	; read current thread flags
    andi r18, Z_THREAD_PRIO_COOP | Z_THREAD_SCHED_LOCKED_MSK

	/* 
	 * scheduler is locked: do not switch thread then exit interrupt
	 */
    brne __exit_kernel_int

	/*
	 * prepare return context for preempted thread
	 * so that __intctx_restore is called when returning from z_yield
	 */
    ldi r24, pm_lo8(__intctx_restore)
    ldi r25, pm_hi8(__intctx_restore)
    push r24
    push r25
#if defined(__AVR_3_BYTE_PC__)
    push r1
#endif 
	/* Finally call the scheduler */
    jmp z_yield
#endif

__exit_kernel_int:
#if CONFIG_KERNEL_COOPERATIVE_THREADS && CONFIG_KERNEL_SCHEDULER_DEBUG
	/*
	 * Write a 'c' to the serial port to indicate that the scheduler
	 * is locked and no thread switch is being made.
	 */
    ldi r24, 0x63	    ; 'c'
    call serial_transmit
#endif

__intctx_restore:
    pop    r31
    pop    r30
    pop    r27
    pop    r26
    pop    r25
    pop    r24
    pop    r23
    pop    r22
    pop    r21
    pop    r20
    pop    r19

__intctx_restore_minimal:
#if CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
	/*
	 * Toggle the GPIO B pin specified by CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
	 * on each system tick exit.
	 */
    ldi     r18, CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
    sts     PINB, r18
#endif

    pop    r18
    pop    r0
    sts    SREG, r0
    pop    r0
    pop    r1
    reti

z_yield:
	/*
	 * call scheduler to determine which thread is next
	 */
    call z_scheduler

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