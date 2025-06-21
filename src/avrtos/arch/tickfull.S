/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

; #if !CONFIG_TICKLESS

/*
 * Select the appropriate timer interrupt vector based on the
 * configuration.
 */
#if CONFIG_KERNEL_SYSLOCK_HW_TIMER == 0
#define Z_SYSCLOCK_ISR TIMER0_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 1
#define Z_SYSCLOCK_ISR TIMER1_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 2
#define Z_SYSCLOCK_ISR TIMER2_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 3
#define Z_SYSCLOCK_ISR TIMER3_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 4
#define Z_SYSCLOCK_ISR TIMER4_COMPA_vect
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER == 5
#define Z_SYSCLOCK_ISR TIMER5_COMPA_vect
#endif

.global z_yield
.global Z_SYSCLOCK_ISR

.extern z_ker
.extern z_sched_enter		; void (void)

/*
 * This is our system tick interrupt handler, which is called at
 * the period specified by CONFIG_KERNEL_SYSCLOCK_PERIOD_US
 */
Z_SYSCLOCK_ISR:
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
#endif /* CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS */
#endif /* CONFIG_KERNEL_TICKS_COUNTER */

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

; #endif /* !CONFIG_TICKLESS */