/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_ASSERT_H_
#define _AVRTOS_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_internals.h"

#include <stdbool.h>
#include <stddef.h>

#include <avr/io.h>
#include <avrtos/sys.h>
#include <util/atomic.h>

// modules
#define K_MODULE_KERNEL	  1
#define K_MODULE_ARCH	  2
#define K_MODULE_SYSCLOCK 3
#define K_MODULE_THREAD	  4
#define K_MODULE_IDLE	  5

#define K_MODULE_MUTEX	   10
#define K_MODULE_SEMAPHORE 11
#define K_MODULE_SIGNAL	   12
#define K_MODULE_WORKQUEUE 13
#define K_MODULE_FIFO	   14
#define K_MODULE_MEMSLAB   15
#define K_MODULE_TIMER	   16
#define K_MODULE_MSGQ	   17
#define K_MODULE_EVENT	   18

#define K_MODULE_DRIVERS_USART	19
#define K_MODULE_DRIVERS_TIMERS 20

#define K_MODULE_APPLICATION 32

// assertions codes
#define K_ASSERT_UNDEFINED 0
#define K_ASSERT_ANY	   0

#define K_ASSERT_INTERRUPT   1
#define K_ASSERT_NOINTERRUPT 2

#define K_ASSERT_LEASTONE_RUNNING 3
#define K_ASSERT_THREAD_STATE	  4

#define K_ASSERT_TRUE  5
#define K_ASSERT_FALSE 6

#define K_ASSERT_NOTNULL 15
#define K_ASSERT_NULL	 16

#define K_ASSERT_PREEMPTIVE   17
#define K_ASSERT_COOPERATIVE  18
#define K_ASSERT_SCHED_LOCKED 19

#define K_ASSERT_ISTHREADIDLE 20

#define K_ASSERT_USER_MODE   21
#define K_ASSERT_KERNEL_MODE 22

#define K_ASSERT_THREAD_READY	23
#define K_ASSERT_THREAD_PENDING 24
#define K_ASSERT_THREAD_STOPPED 25
#define K_ASSERT_THREAD_IDLE	26

#define K_ASSERT_UNDEFINED_LINE 0u

#if CONFIG_KERNEL_ASSERT
#define __ASSERT(_acode, _assertion) \
	__assert((uint8_t)(_assertion), K_MODULE, _acode, __LINE__)
#define __ASSERT_APP(_assertion) \
	__assert((uint8_t)(_assertion), K_MODULE_APPLICATION, K_ASSERT_ANY, __LINE__)
#else
#define __ASSERT(_acode, _assertion)
#define __ASSERT_APP(_assertion)
#endif

#define K_ASSERT(_acode, _assertion) __ASSERT(_acode, _assertion)
#define K_ASSERT_APP(_assertion)     __ASSERT_APP(_assertion)

#define __ASSERT_TRUE(test)  __ASSERT(K_ASSERT_TRUE, (test) != 0)
#define __ASSERT_FALSE(test) __ASSERT(K_ASSERT_FALSE, (test) == 0)

#define __ASSERT_INTERRUPT()   __ASSERT(K_ASSERT_INTERRUPT, z_interrupts() != 0)
#define __ASSERT_NOINTERRUPT() __ASSERT(K_ASSERT_NOINTERRUPT, z_interrupts() == 0)

#define __ASSERT_NOTNULL(var) __ASSERT(K_ASSERT_NOTNULL, (var) != NULL)
#define __ASSERT_NULL(var)    __ASSERT(K_ASSERT_NULL, (var) == NULL)

#define __ASSERT_LEASTONE_RUNNING() \
	__ASSERT(K_ASSERT_LEASTONE_RUNNING, k_ready_count() != 0u)
#define __ASSERT_THREAD_STATE(thread, th_state) \
	__ASSERT(K_ASSERT_THREAD_STATE, (thread->flags & Z_THREAD_STATE_MSK) == th_state)
#define __ASSERT_THREAD_NOT_STATE(thread, th_state) \
	__ASSERT(K_ASSERT_THREAD_STATE, (thread->flags & Z_THREAD_STATE_MSK) != th_state)

#define __ASSERT_PREEMPTIVE()	__ASSERT(K_ASSERT_PREEMPTIVE, k_cur_is_preempt())
#define __ASSERT_COOPERATIVE()	__ASSERT(K_ASSERT_COOPERATIVE, k_cur_is_coop())
#define __ASSERT_SCHED_LOCKED() __ASSERT(K_ASSERT_SCHED_LOCKED, k_sched_locked())

#define __ASSERT_ISR_CONTEXT()
#define __ASSERT_THREAD_CONTEXT()

/**
 * @brief Assert evaluated expression is not zero
 *
 * @param expression Expression to evaluate
 * @param module Module whithin the assertion is made (K_MODULE_*)
 * @param acode Assertion code (K_ASSERT_*)
 * @param line Line number of the assertion within the source file
 */
void __assert(uint8_t expression, uint8_t module, uint8_t acode, uint16_t line);

#ifdef __cplusplus
}
#endif

#endif