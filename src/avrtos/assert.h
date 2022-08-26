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

#include <stdbool.h>
#include <stddef.h>

#include <avr/io.h>
#include <util/atomic.h>

#include "avrtos.h"

/*___________________________________________________________________________*/

#define K_MODULE_KERNEL         1
#define K_MODULE_ARCH           2
#define K_MODULE_SYSCLOCK       3
#define K_MODULE_THREAD         4
#define K_MODULE_IDLE           5

#define K_MODULE_MUTEX          10
#define K_MODULE_SEMAPHORE      11
#define K_MODULE_SIGNAL         12
#define K_MODULE_WORKQUEUE      13
#define K_MODULE_FIFO           14
#define K_MODULE_MEMSLAB        15
#define K_MODULE_TIMER          16
#define K_MODULE_MSGQ           17
#define K_MODULE_EVENT          18

#define K_MODULE_DRIVERS_USART  19
#define K_MODULE_DRIVERS_TIMERS  20

#define K_MODULE_APPLICATION    0x20

/*___________________________________________________________________________*/

#define K_ASSERT_UNDEFINED              0

#define K_ASSERT_INTERRUPT              1
#define K_ASSERT_NOINTERRUPT            2

#define K_ASSERT_LEASTONE_RUNNING       3
#define K_ASSERT_THREAD_STATE           4

#define K_ASSERT_TRUE                   5
#define K_ASSERT_FALSE                  6

#define K_ASSERT_NOTNULL               	15
#define K_ASSERT_NULL                  	16

#define K_ASSERT_PREEMPTIVE	     	17
#define K_ASSERT_COOPERATIVE	     	18
#define K_ASSERT_SCHED_LOCKED	     	19

#define K_ASSERT_ISTHREADIDLE	     	20

/*___________________________________________________________________________*/

// move to assert.h
#if KERNEL_ASSERT
#   	define __ASSERT(acode, assertion) __assert((uint8_t) (assertion), K_MODULE, acode, __LINE__)
#else
#   	define __ASSERT(acode, assertion) 
#endif

#define K_ASSERT __ASSERT

/*___________________________________________________________________________*/

#define __STATIC_ASSERT(test_for_true, msg) _Static_assert(test_for_true, msg)
#define __STATIC_ASSERT_AUTOMSG(test_for_true) _Static_assert(test_for_true, "(" #test_for_true ") failed")

#define K_STATIC_ASSERT __STATIC_ASSERT_AUTOMSG

/*___________________________________________________________________________*/

#define __ASSERT_TRUE(test)                 __ASSERT(K_ASSERT_TRUE, (test) != 0)
#define __ASSERT_FALSE(test)                __ASSERT(K_ASSERT_FALSE, (test) == 0)

#define __ASSERT_INTERRUPT()                __ASSERT(K_ASSERT_INTERRUPT, __k_interrupts() != 0)
#define __ASSERT_NOINTERRUPT()              __ASSERT(K_ASSERT_NOINTERRUPT, __k_interrupts() == 0)

#define __ASSERT_NOTNULL(var)               __ASSERT(K_ASSERT_NOTNULL, (var) != NULL)
#define __ASSERT_NULL(var)                  __ASSERT(K_ASSERT_NULL, (var) == NULL)

#define __ASSERT_LEASTONE_RUNNING()         __ASSERT(K_ASSERT_LEASTONE_RUNNING, k_ready_count() != 0u)
#define __ASSERT_THREAD_STATE(th, th_state) __ASSERT(K_ASSERT_THREAD_STATE, th->state == th_state)

#define __ASSERT_PREEMPTIVE()               __ASSERT(K_ASSERT_PREEMPTIVE, k_cur_is_preempt())
#define __ASSERT_COOPERATIVE()              __ASSERT(K_ASSERT_COOPERATIVE, k_cur_is_coop())
#define __ASSERT_SCHED_LOCKED()             __ASSERT(K_ASSERT_SCHED_LOCKED, k_sched_locked())

/*___________________________________________________________________________*/

void __assert(uint8_t expression, uint8_t module, uint8_t acode, uint16_t line);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif