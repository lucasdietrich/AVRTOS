/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_ALLOC_PRIVATE_H
#define _AVRTOS_ALLOC_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define ALIGN_MASK(align) ((1u << align) - 1u)
#define ALIGN_PTR(ptr, align)                                                            \
	((void *)((((uint16_t)ptr) + ALIGN_MASK(align)) & ~ALIGN_MASK(align)))

#define K_NO_ALIGN 0u
#define K_ALIGN_2  1u
#define K_ALIGN_4  2u
#define K_ALIGN_8  3u
#define K_ALIGN_16 4u
#define K_ALIGN_32 5u
#define K_ALIGN_64 6u

#ifdef __cplusplus
}
#endif

#endif