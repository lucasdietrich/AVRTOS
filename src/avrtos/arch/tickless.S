/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/defines.h>

#if CONFIG_KERNEL_TICKLESS && 1

#define Z_TICKLESS_ISR TIMER1_COMPA_vect

.global z_yield
.global Z_TICKLESS_ISR

.extern z_ker
.extern z_sched_point_enter		; void (void)

Z_TICKLESS_ISR:
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

	/*
	 * Call scheduler to evaluate timeouts for threads/timers and events.
	 * Schedule threads consequently. 
	 */
    call z_sched_point_enter

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
    pop    r18
    pop    r0
    sts    SREG, r0
    pop    r0
    pop    r1
    reti

#endif /* CONFIG_KERNEL_TICKLESS */