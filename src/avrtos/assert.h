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

#define K_MODULE_APPLICATION    0x20

/*___________________________________________________________________________*/

#define K_ASSERT_UNDEFINED              0

#define K_ASSERT_INTERRUPT              1
#define K_ASSERT_NOINTERRUPT            2

#define K_ASSERT_LEASTONE_RUNNING       3
#define K_ASSERT_THREAD_STATE           4

#define K_ASSERT_TRUE                   5
#define K_ASSERT_FALSE                  6

#define K_ASSERT_WORKQUEUE              10

#define K_ASSSERT_NOTNULL               15
#define K_ASSSERT_NULL                  16

/*___________________________________________________________________________*/

// move to assert.h
#if KERNEL_ASSERT
#   define __ASSERT(acode, assertion) __assert((uint8_t) (assertion), K_MODULE, acode, __LINE__)
#else
#   define __ASSERT(acode, assertion) 
#endif

/*___________________________________________________________________________*/

#define __ASSERT_TRUE(test)                 __ASSERT(K_ASSERT_TRUE, (test) != 0)
#define __ASSERT_FALSE(test)                __ASSERT(K_ASSERT_FALSE, (test) == 0)

#define __ASSERT_INTERRUPT()                __ASSERT(K_ASSERT_INTERRUPT, __k_interrupts() != 0)
#define __ASSERT_NOINTERRUPT()              __ASSERT(K_ASSERT_NOINTERRUPT, __k_interrupts() == 0)

#define __ASSERT_NOTNULL(var)               __ASSERT(K_ASSSERT_NOTNULL, (var) != NULL)
#define __ASSERT_NULL(var)                  __ASSERT(K_ASSSERT_NULL, (var) == NULL)

#define __ASSERT_LEASTONE_RUNNING()         __ASSERT(K_ASSERT_LEASTONE_RUNNING, !_k_runqueue_single())
#define __ASSERT_THREAD_STATE(th, th_state) __ASSERT(K_ASSERT_THREAD_STATE, th->state == th_state)

/*___________________________________________________________________________*/

void __assert(uint8_t expression, uint8_t module, uint8_t acode, uint16_t line);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif