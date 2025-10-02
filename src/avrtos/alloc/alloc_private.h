/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
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

#define Z_ALIGN_MASK(align) (align - 1u)
#define Z_ALIGN_PTR(ptr, align)                                                          \
    ((void *)((((uint16_t)ptr) + Z_ALIGN_MASK(align)) & ~Z_ALIGN_MASK(align)))

#define Z_NO_ALIGN 1u
#define Z_ALIGN_2  2u
#define Z_ALIGN_4  4u
#define Z_ALIGN_8  8u
#define Z_ALIGN_16 16u
#define Z_ALIGN_32 32u
#define Z_ALIGN_64 64u

#ifdef __cplusplus
}
#endif

#endif