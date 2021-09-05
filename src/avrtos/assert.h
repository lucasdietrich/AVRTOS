#ifndef _AVRTOS_ASSERT_H_
#define _AVRTOS_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include <util/atomic.h>

#include "multithreading.h"
#include "misc/uart.h"


/*___________________________________________________________________________*/

// move to assert.h
#if KERNEL_ASSERT
#   define __ASSERT(assertion, code) __assert((bool) (assertion), code)
#else
#   define __ASSERT(assertion, code) 
#endif

/*___________________________________________________________________________*/

// code between 1 and 127 are internal assert codes
// code between 128 and 255 are user assert codes

#define K_ASSERT_USER(n)      ((n & 0xFF) << 8)

// interrupts enabled
#define K_ASSERT_INTERRUPT          1
#define __ASSERT_INTERRUPT(loc)     __ASSERT(SREG & (1 << SREG_I), K_ASSERT_USER(loc) | K_ASSERT_INTERRUPT)

// interrupt disabled
#define K_ASSERT_NOINTERRUPT        2
#define __ASSERT_NOINTERRUPT(loc)   __ASSERT((SREG & (1 << SREG_I)) == 0, K_ASSERT_USER(loc) | K_ASSERT_NOINTERRUPT)

// runqueue has more than 1 thread running
#define K_ASSERT_LEASTONE_RUNNING   3
#define __ASSERT_LEASTONE_RUNNING(loc)   __ASSERT(!_k_runqueue_single(), K_ASSERT_USER(loc) | K_ASSERT_LEASTONE_RUNNING)

#define K_ASSERT_WORKQUEUE          4

/*___________________________________________________________________________*/

void __assert(bool expression, uint16_t acode);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif