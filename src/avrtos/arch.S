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

.extern z_scheduler
.extern z_current
.extern z_ticks 			; u32/u40
.extern z_sched_ticks_remaining 	; u8
.extern z_system_shift

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

.global TIMERn_COMPA_vect

TIMERn_COMPA_vect:
	push	r1
	push	r0
	lds	r0, SREG
	push	r0
	eor	r1, r1
	push	r18

#if CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
	ldi 	r18, CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
	sts 	PINB, r18
#endif /* CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG */

#if CONFIG_KERNEL_TICKS_COUNTER
	ldi r18, 0x01			; 1 cycle

	lds r0, z_ticks		; 2 cycles
	add r0, r18			; 1 cycle
	sts z_ticks, r0		; 2 cycles

	lds r0, z_ticks + 1		; 2 cycles
	adc r0, r1			; 1 cycle
	sts z_ticks + 1, r0		; 2 cycles
	
	lds r0, z_ticks + 2		; 2 cycles
	adc r0, r1			; 1 cycle
	sts z_ticks + 2, r0		; 2 cycles

	lds r0, z_ticks + 3		; 2 cycles
	adc r0, r1			; 1 cycle
	sts z_ticks + 3, r0		; 2 cycles

#if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
	lds r0, z_ticks + 4		; 2 cycles
	adc r0, r1			; 1 cycle
	sts z_ticks + 4, r0		; 2 cycles
#endif
; CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
#endif
; CONFIG_KERNEL_TICKS_COUNTER

#if CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS
	lds r18, z_sched_ticks_remaining
	subi r18, 0x01
	sts z_sched_ticks_remaining, r18
	brne __intctx_restore_minimal
#endif 
;CONFIG_KERNEL_TIME_SLICE_MULTIPLE_TICKS

	push	r19
	push	r20
	push	r21
	push	r22
	push	r23
	push	r24
	push	r25
	push	r26
	push	r27
	push	r30
	push	r31

#if CONFIG_KERNEL_DEBUG
	ldi r24, 0x2e           ; '.'
	call serial_transmit
#endif
; CONFIG_KERNEL_DEBUG

	call z_system_shift

; check if scheduler is locked (cooperative thread or temporarely locked)
#if CONFIG_KERNEL_COOPERATIVE_THREADS
	lds ZL, z_current           ; load current thread addr in Z
	lds ZH, z_current + 1

	ldd r18, Z + 2      ; read flag
	andi r18, Z_FLAG_COOP | Z_FLAG_SCHED_LOCKED

	; scheduler is locked: do not switch thread and exit interrupt
	brne __sched_locked

	; prepare return context for preempted thread
	; so that __intctx_restore is called when returning from z_yield
	ldi r24, pm_lo8(__intctx_restore)
	ldi r25, pm_hi8(__intctx_restore)
	push r24
	push r25
#if defined(__AVR_3_BYTE_PC__)
	push r1
#endif 
	jmp z_yield
#endif
; CONFIG_KERNEL_COOPERATIVE_THREADS

__sched_locked:
#if CONFIG_KERNEL_COOPERATIVE_THREADS && CONFIG_KERNEL_SCHEDULER_DEBUG
	ldi r24, 0x63	    ; 'c'
	call serial_transmit
#endif

__intctx_restore:
	pop	r31
	pop	r30
	pop	r27
	pop	r26
	pop	r25
	pop	r24
	pop	r23
	pop	r22
	pop	r21
	pop	r20
	pop	r19

__intctx_restore_minimal:
#if CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
	ldi 	r18, CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
	sts 	PINB, r18
#endif /* CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG */
	pop	r18
	pop	r0
	sts	SREG, r0
	pop	r0
	pop	r1
	reti

.global z_yield
z_yield:
	; call scheduler to determine which thread is next
	call z_scheduler

	; r24 contain previous thread
	movw r26, r24
	
	; current thread
	lds r30, z_current
	lds r31, z_current + 1

#if CONFIG_KERNEL_SCHEDULER_COMPARE_THREADS_BEFORE_SWITCH
	; Compare threads addresses after scheduler call to prevent thread switch to the same thread
	; - i.e.thread switch only if "previous" and "z_current" threads are different
	; - inefficient if at least two threads are always ready
	; - compare low bytes before high bytes because 
	;     they have a higher probability to be different
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
	ldi 	r16, CONFIG_KERNEL_SYSTICK_GPIOB_DEBUG
	sts 	PINB, r16
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

.extern k_stop
.extern __fault
z_thread_entry:
	movw r24, r2
	movw r30, r4
	sts SREG, r6
	
#if CONFIG_KERNEL_THREAD_TERMINATION_TYPE == 0
	ijmp
#elif CONFIG_KERNEL_THREAD_TERMINATION_TYPE == -1
	icall
	ldi r24, 0x03 ; K_THREAD_TERMINATED
	call __fault
#else
	icall
	call k_stop
#endif


.global z_interrupts
z_interrupts:
    ldi r24, 0x00
    lds r25, SREG
    sbrc r25, SREG_I
    ldi r24, 0x01
    ret


#if CONFIG_KERNEL_TICKS_COUNTER
.global k_ticks_get_32
.global k_ticks_get_64
k_ticks_get_32:
	lds	r26, SREG
	cli
	lds 	r22, z_ticks
	lds 	r23, z_ticks + 1
	lds 	r24, z_ticks + 2
	sts	SREG, r26
	lds 	r25, z_ticks + 3
	ret

k_ticks_get_64:
	lds	r26, SREG
	cli
	lds 	r18, z_ticks
	lds 	r19, z_ticks + 1
	lds 	r20, z_ticks + 2
	lds 	r21, z_ticks + 3
	sts	SREG, r26
#if CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS
	lds	r22, z_ticks + 4
#else
	ldi	r22, 0x00
#endif /* CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS */
	ldi	r23, 0x00
	ldi	r24, 0x00
	ldi	r25, 0x00
	ret
#endif /* CONFIG_KERNEL_TICKS_COUNTER */

.global __debug
__debug:
	ret

#if CONFIG_KERNEL_DEBUG_PREEMPT_UART
.global USART0_RX_vect
USART0_RX_vect:
	push	r1
	push	r0
	lds	r0, SREG
	push	r0
	eor	r1, r1
	push	r18
	push	r19
	push	r20
	push	r21
	push	r22
	push	r23
	push	r24
	push	r25
	push	r26
	push	r27
	push	r30
	push	r31

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