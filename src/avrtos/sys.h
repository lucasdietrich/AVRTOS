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

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

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

/**
 * @brief Colorize a pointer by embedding a color value in the unused bits.
 *
 * This macro takes a pointer and a color value, and returns a new pointer
 * with the color embedded in the higher bits that are typically unused
 * in AVR pointers (which are 16-bit but address 14-bit space).
 *
 * @param ptr The original pointer to colorize.
 * @param color The color value to embed (0-3).
 * @param ptr_width The bit width of the pointer (typically Z_ARCH_PTR_SIZE).
 * @return The colorized pointer.
 */
#define __sys_ptr_colored(ptr, color, ptr_width)                                         \
    ((void *)((uint16_t)(ptr) | ((uint16_t)(color) << (ptr_width))))

/**
 * @brief Decolorize a pointer by removing the embedded color value.
 *
 * This macro takes a colorized pointer and returns the original pointer
 * with the color bits masked out.
 *
 * @param ptr The colorized pointer.
 * @param ptr_width The bit width of the pointer (typically Z_ARCH_PTR_SIZE).
 * @return The decolorized pointer.
 */
#define __sys_ptr_decolored(ptr, ptr_width)                                              \
    ((void *)((uint16_t)(ptr) & ((1 << (ptr_width)) - 1)))

/**
 * @brief Colorize a pointer in-place.
 *
 * This macro modifies the pointer variable to embed the color value.
 *
 * @param ptr The pointer variable to colorize.
 * @param color The color value to embed.
 * @param ptr_width The bit width of the pointer.
 */
#define __sys_ptr_colorize(ptr, color, ptr_width)                                        \
    ptr = __sys_ptr_colored(ptr, color, ptr_width)

/**
 * @brief Decolorize a pointer in-place.
 *
 * This macro modifies the pointer variable to remove the color value.
 *
 * @param ptr The pointer variable to decolorize.
 * @param ptr_width The bit width of the pointer.
 */
#define __sys_ptr_decolorize(ptr, ptr_width) ptr = __sys_ptr_decolored(ptr, ptr_width)

/**
 * @brief Extract the color value from a colorized pointer.
 *
 * @param ptr The colorized pointer.
 * @param ptr_width The bit width of the pointer.
 * @return The embedded color value.
 */
#define __sys_ptr_color(ptr, ptr_width) ((uint8_t)((uint16_t)(ptr) >> (ptr_width)))

/**
 * @brief Architecture-specific pointer size in bits.
 *
 * For AVR, this is 14 bits since AVR has 16-bit pointers but only addresses
 * 16KB of SRAM (2^14 = 16384 bytes).
 */
#define Z_ARCH_PTR_SIZE 14

/**
 * @brief Colorize a pointer using the architecture-specific pointer width.
 *
 * @note It's unsafe to colorize flash pointers, only SRAM pointers should be colorized.
 *
 * @param ptr The pointer to colorize.
 * @param color The color value to embed.
 */
#define sys_ptr_colorize(ptr, color) __sys_ptr_colorize(ptr, color, Z_ARCH_PTR_SIZE)

/**
 * @brief Decolorize a pointer using the architecture-specific pointer width.
 *
 * @param ptr The pointer to decolorize.
 */
#define sys_ptr_decolorize(ptr) __sys_ptr_decolorize(ptr, Z_ARCH_PTR_SIZE)

/**
 * @brief Get the color value from a colorized pointer.
 *
 * @param ptr The colorized pointer.
 * @return The embedded color value.
 */
#define sys_ptr_color(ptr) __sys_ptr_color(ptr, Z_ARCH_PTR_SIZE)

/**
 * @brief Get the decolorized version of a pointer.
 *
 * @param ptr The potentially colorized pointer.
 * @return The decolorized pointer.
 */
#define sys_ptr_decolored(ptr) __sys_ptr_decolored(ptr, Z_ARCH_PTR_SIZE)

/**
 * @brief Create a colorized pointer without modifying the original.
 *
 * @param ptr The original pointer.
 * @param color The color value to embed.
 * @return The colorized pointer.
 */
#define sys_ptr_colored(ptr, color) __sys_ptr_colored(ptr, color, Z_ARCH_PTR_SIZE)

/**
 * @brief Pointer color constants.
 */
#define SYS_PTR_COLOR_NONE  0u /**< No color */
#define SYS_PTR_COLOR_BLUE  1u /**< Blue color */
#define SYS_PTR_COLOR_RED   2u /**< Red color */
#define SYS_PTR_COLOR_GREEN 3u /**< Green color */

#endif