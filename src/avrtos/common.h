/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_COMMON_H
#define _AVRTOS_COMMON_H

/*___________________________________________________________________________*/

/* Compiler specific */

#define __noinline            	__attribute__((noinline))
#define __noreturn            	__attribute__((__noreturn__))
#define CODE_UNREACHABLE    	__builtin_unreachable();
#define __always_inline 	__attribute__((always_inline)) inline
#define __noinit 	    	__attribute__((section(".noinit")))
#define __bss 	    		__attribute__((section(".bss")))

#define __STATIC_ASSERT(test_for_true, msg) \
	_Static_assert(test_for_true, msg)
#define __STATIC_ASSERT_NOMSG(test_for_true) \
	_Static_assert(test_for_true, "(" #test_for_true ") failed")

/*___________________________________________________________________________*/

#define HTONL(n) ((((((uint32_t)(n) & 0xFF)) << 24) |      \
               ((((uint32_t)(n) & 0xFF00)) << 8) |         \
               ((((uint32_t)(n) & 0xFF0000)) >> 8) |       \
               ((((uint32_t)(n) & 0xFF000000)) >> 24)))

#define HTONS(n) (((((uint16_t)(n) & 0xFF)) << 8) |       \
               ((((uint16_t)(n) & 0xFF00)) >> 8))

#define K_SWAP_ENDIANNESS(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))

#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))

#define BIT(b) (1 << (b))
#define SET_BIT(x, b)  ((x) |= b)
#define CLR_BIT(x, b)  ((x) &= (~(b)))
#define TEST_BIT(x, b) ((bool) ((x) & b))

#define ARG_UNUSED(arg) ((void) arg)

#define IN_RANGE(x, a, b) ((x >= a) && (x <= b))

/*___________________________________________________________________________*/

/* Note: Willingly not adding unsigned "u" suffix to numbers
 * because these flags are used in assembly code
 */

#define K_FLAG_STATE_POS		0
#define K_FLAG_STOPPED			(0 << K_FLAG_STATE_POS)
#define K_FLAG_READY			(1 << K_FLAG_STATE_POS)
#define K_FLAG_PENDING			(2 << K_FLAG_STATE_POS)
#define K_FLAG_IDLE			(3 << K_FLAG_STATE_POS)

#define K_FLAG_SCHED_LOCKED_POS 	2
#define K_FLAG_SCHED_LOCKED     	(1 << K_FLAG_SCHED_LOCKED_POS)

#define K_FLAG_PRIO_POS      		3
#define K_MASK_PRIO			(3 << K_FLAG_PRIO_POS)
#define K_MASK_PRIO_COOP		(2 << K_FLAG_PRIO_POS)
#define K_MASK_PRIO_LEVEL		(1 << K_FLAG_PRIO_POS)
#define K_FLAG_COOP      		(2 << K_FLAG_PRIO_POS)
#define K_FLAG_PREEMPT          	(0 << K_FLAG_PRIO_POS)
#define K_FLAG_PRIO_HIGH		(1 << K_FLAG_PRIO_POS)
#define K_FLAG_PRIO_LOW			(0 << K_FLAG_PRIO_POS)

#define K_FLAG_TIMER_EXPIRED_POS  	5
#define K_FLAG_TIMER_EXPIRED        	(1 << K_FLAG_TIMER_EXPIRED_POS)

#define K_FLAG_PEND_CANCELED_POS 	6
#define K_FLAG_PEND_CANCELED		(1 << K_FLAG_PEND_CANCELED_POS)

#define K_FLAG_WAKEUP_SCHED_POS 	7
#define K_FLAG_WAKEUP_SCHED		(1 << K_FLAG_PEND_CANCK_FLAG_PEND_WAKEUPSCHED_POSELED_POS)

#define K_COOPERATIVE          	 	K_FLAG_COOP
#define K_PREEMPTIVE           	 	K_FLAG_PREEMPT
#define K_PRIO_DEFAULT          	(K_COOPERATIVE | K_FLAG_PRIO_LOW)
/*___________________________________________________________________________*/

#endif