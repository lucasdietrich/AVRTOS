/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_COMMON_H
#define _AVRTOS_COMMON_H

/*___________________________________________________________________________*/

#define Z_STRINGIFY(x) #x
#define STRINGIFY(s)   Z_STRINGIFY(s)

#define _DO_CONCAT(x, y) x##y
#define _CONCAT(x, y)	 _DO_CONCAT(x, y)

/*___________________________________________________________________________*/

/* Compiler specific */

#define Z_LINK_SECTION(_section) __attribute__((section(Z_STRINGIFY(_section))))

#define __noinline	 __attribute__((noinline))
#define __noreturn	 __attribute__((__noreturn__))
#define CODE_UNREACHABLE __builtin_unreachable();
#define __always_inline	 __attribute__((always_inline)) inline
#define __noinit	 Z_LINK_SECTION(.noinit)
#define __bss		 Z_LINK_SECTION(.bss)

#define __STATIC_ASSERT(test_for_true, msg) _Static_assert(test_for_true, msg)
#define __STATIC_ASSERT_NOMSG(test_for_true)                                             \
	_Static_assert(test_for_true, "(" #test_for_true ") failed")

/*___________________________________________________________________________*/

#define HTONL(n)                                                                         \
	((((((uint32_t)(n)&0xFF)) << 24) | ((((uint32_t)(n)&0xFF00)) << 8) |             \
	  ((((uint32_t)(n)&0xFF0000)) >> 8) | ((((uint32_t)(n)&0xFF000000)) >> 24)))

#define HTONS(n) (((((uint16_t)(n)&0xFF)) << 8) | ((((uint16_t)(n)&0xFF00)) >> 8))

#define K_SWAP_ENDIANNESS(n)                                                             \
	(((((uint16_t)(n)&0xFF)) << 8) | (((uint16_t)(n)&0xFF00) >> 8))

#define MIN(a, b)		       ((a < b) ? (a) : (b))
#define MAX(a, b)		       ((a > b) ? (a) : (b))
#define ARRAY_SIZE(array)	       (sizeof(array) / sizeof(array[0]))
#define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))

#define BIT(b)	       (1 << (b))
#define SET_BIT(x, b)  ((x) |= b)
#define CLR_BIT(x, b)  ((x) &= (~(b)))
#define TEST_BIT(x, b) ((bool)((x)&b))

#define ARG_UNUSED(arg) ((void)arg)

#define IN_RANGE(x, a, b) ((x >= a) && (x <= b))

#define sys_read_le16(addr)	  (*(uint16_t *)(addr))
#define sys_write_le16(addr, val) (*(uint16_t *)(addr) = (val))
#define sys_read_le32(addr)	  (*(uint32_t *)(addr))
#define sys_write_le32(addr, val) (*(uint32_t *)(addr) = (val))

#define sys_read_be16(addr)                                                              \
	((uint16_t)(*(uint8_t *)(addr) << 8) | *(uint8_t *)((addr) + 1))
#define sys_write_be16(addr, val)                                                        \
	(*(uint8_t *)(addr)	  = (uint8_t)((val) >> 8),                               \
	 *(uint8_t *)((addr) + 1) = (uint8_t)(val))
#define sys_read_be32(addr)                                                              \
	((uint32_t)(*(uint8_t *)(addr) << 24) | (*(uint8_t *)((addr) + 1) << 16) |       \
	 (*(uint8_t *)((addr) + 2) << 8) | *(uint8_t *)((addr) + 3))
#define sys_write_be32(addr, val)                                                        \
	(*(uint8_t *)(addr)	  = (uint8_t)((val) >> 24),                              \
	 *(uint8_t *)((addr) + 1) = (uint8_t)((val) >> 16),                              \
	 *(uint8_t *)((addr) + 2) = (uint8_t)((val) >> 8),                               \
	 *(uint8_t *)((addr) + 3) = (uint8_t)(val))

#define sys_ptr_diff(a, b)	     ((uint16_t)((char *)(a) - (char *)(b)))
#define sys_ptr_add(ptr, offset)     ((void *)((char *)(ptr) + (offset)))
#define sys_ptr_shift(ptr_p, offset) (*(ptr_p) = sys_ptr_add(*(ptr_p), offset))

/*___________________________________________________________________________*/

/* Note: Willingly not adding unsigned "u" suffix to numbers
 * because these flags are used in assembly code
 */

#define Z_FLAG_STATE_POS 0
#define Z_MASK_STATE	 (3 << Z_FLAG_STATE_POS)
#define Z_FLAG_STOPPED	 (0 << Z_FLAG_STATE_POS)
#define Z_FLAG_READY	 (1 << Z_FLAG_STATE_POS)
#define Z_FLAG_PENDING	 (2 << Z_FLAG_STATE_POS)
#define Z_FLAG_IDLE	 (3 << Z_FLAG_STATE_POS)

#define Z_FLAG_SCHED_LOCKED_POS 2
#define Z_FLAG_SCHED_LOCKED	(1 << Z_FLAG_SCHED_LOCKED_POS)

#define Z_FLAG_PRIO_POS	  3
#define Z_MASK_PRIO	  (3 << Z_FLAG_PRIO_POS)
#define Z_MASK_PRIO_COOP  (2 << Z_FLAG_PRIO_POS)
#define Z_MASK_PRIO_LEVEL (1 << Z_FLAG_PRIO_POS)
#define Z_FLAG_COOP	  (2 << Z_FLAG_PRIO_POS)
#define Z_FLAG_PREEMPT	  (0 << Z_FLAG_PRIO_POS)
#define Z_FLAG_PRIO_HIGH  (1 << Z_FLAG_PRIO_POS)
#define Z_FLAG_PRIO_LOW	  (0 << Z_FLAG_PRIO_POS)

#define Z_FLAG_TIMER_EXPIRED_POS 5
#define Z_FLAG_TIMER_EXPIRED	 (1 << Z_FLAG_TIMER_EXPIRED_POS)

#define Z_FLAG_PEND_CANCELED_POS 6
#define Z_FLAG_PEND_CANCELED	 (1 << Z_FLAG_PEND_CANCELED_POS)

#define Z_FLAG_WAKEUP_SCHED_POS 7
#define Z_FLAG_WAKEUP_SCHED	(1 << Z_FLAG_WAKEUP_SCHED_POS)

#define K_COOPERATIVE  Z_FLAG_COOP
#define K_PREEMPTIVE   Z_FLAG_PREEMPT
#define K_PRIO_DEFAULT (K_COOPERATIVE | Z_FLAG_PRIO_LOW)
/*___________________________________________________________________________*/

#endif