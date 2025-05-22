/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Platform macros and definitions.
 *
 * The architecture, the language and the compiler defines the platform in the
 * current context:
 * - AVR / C / avr-gcc
 */

#ifndef _AVRTOS_COMMON_H
#define _AVRTOS_COMMON_H

/* misc */

#define Z_STRINGIFY(x) #x
#define STRINGIFY(s)   Z_STRINGIFY(s)

#define _DO_CONCAT(x, y) x##y
#define _CONCAT(x, y)    _DO_CONCAT(x, y)

#define ARG_UNUSED(arg) ((void)arg)

/* Compiler specific */

#define Z_LINK_SECTION(_section) __attribute__((section(Z_STRINGIFY(_section))))
#define Z_LINK_SECTION_USED(_section)                                                    \
    __attribute__((used, section(Z_STRINGIFY(_section))))

#define __noinline               __attribute__((noinline))
#define __noreturn               __attribute__((__noreturn__))
#define CODE_UNREACHABLE         __builtin_unreachable();
#define __always_inline          __attribute__((always_inline)) inline
#define __noinit                 Z_LINK_SECTION(.noinit)
#define __bss                    Z_LINK_SECTION(.bss)
#define __packed                 __attribute__((packed))
#define __fallthrough            __attribute__((fallthrough))
#define memory_barrier()         asm volatile("" : : : "memory")
#define memory_barrier_var(_var) asm volatile("" : : "b"(_var) : "memory")

#define __static_assert                     _Static_assert
#define __STATIC_ASSERT(test_for_true, msg) __static_assert(test_for_true, msg)
#define __STATIC_ASSERT_NOMSG(test_for_true)                                             \
    __static_assert(test_for_true, "(" #test_for_true ") failed")

/* bits manipulation */

#define HTONL(n)                                                                         \
    ((((((uint32_t)(n)&0xFF)) << 24) | ((((uint32_t)(n)&0xFF00)) << 8) |                 \
      ((((uint32_t)(n)&0xFF0000)) >> 8) | ((((uint32_t)(n)&0xFF000000)) >> 24)))

#define HTONS(n) (((((uint16_t)(n)&0xFF)) << 8) | ((((uint16_t)(n)&0xFF00)) >> 8))

#define K_SWAP_ENDIANNESS(n)                                                             \
    (((((uint16_t)(n)&0xFF)) << 8) | (((uint16_t)(n)&0xFF00) >> 8))

#define MIN(a, b)                      (((a) < (b)) ? (a) : (b))
#define MAX(a, b)                      (((a) > (b)) ? (a) : (b))
#define ARRAY_SIZE(array)              (sizeof(array) / sizeof(array[0]))
#define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))
#define SIZEOF_MEMBER(type, member)    (sizeof(((type *)0)->member))
#define INDEX_OF(array, element)       ((element) - (array))

#define BIT(b)         (1llu << (b))
#define SET_BIT(x, b)  ((x) |= (b))
#define CLR_BIT(x, b)  ((x) &= (~(b)))
#define TEST_BIT(x, b) ((bool)((x) & (b)))

#define IN_RANGE(x, a, b) (((x) >= (a)) && ((x) <= (b)))

#define sys_read_le16(addr)       (*(uint16_t *)(addr))
#define sys_write_le16(addr, val) (*(uint16_t *)(addr) = (val))
#define sys_read_le32(addr)       (*(uint32_t *)(addr))
#define sys_write_le32(addr, val) (*(uint32_t *)(addr) = (val))

#define sys_read_be16(addr)                                                              \
    ((uint16_t)(*(uint8_t *)(addr) << 8) | *(uint8_t *)((addr) + 1))
#define sys_write_be16(addr, val)                                                        \
    (*(uint8_t *)(addr)       = (uint8_t)((val) >> 8),                                   \
     *(uint8_t *)((addr) + 1) = (uint8_t)(val))
#define sys_read_be32(addr)                                                              \
    ((uint32_t)(*(uint8_t *)(addr) << 24) | (*(uint8_t *)((addr) + 1) << 16) |           \
     (*(uint8_t *)((addr) + 2) << 8) | *(uint8_t *)((addr) + 3))
#define sys_write_be32(addr, val)                                                        \
    (*(uint8_t *)(addr)       = (uint8_t)((val) >> 24),                                  \
     *(uint8_t *)((addr) + 1) = (uint8_t)((val) >> 16),                                  \
     *(uint8_t *)((addr) + 2) = (uint8_t)((val) >> 8),                                   \
     *(uint8_t *)((addr) + 3) = (uint8_t)(val))

#define sys_ptr_diff(a, b)           ((uint16_t)((char *)(a) - (char *)(b)))
#define sys_ptr_add(ptr, offset)     ((void *)((char *)(ptr) + (offset)))
#define sys_ptr_sub(ptr, offset)     ((void *)((char *)(ptr) - (offset)))
#define sys_ptr_inc(ptr)             ((void *)((char *)(ptr) + 1))
#define sys_ptr_dec(ptr)             ((void *)((char *)(ptr)-1))
#define sys_ptr_shift(ptr_p, offset) (*(ptr_p) = sys_ptr_add(*(ptr_p), offset))

#endif