/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_ALLOC_API_H
#define _AVRTOS_ALLOC_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "alloc.h"

/**
 * @brief Allocator statistics structure
 */
struct alloc_stats {
    /**
     * @brief Total allocator memory size
     */
    size_t total;

    /**
     * @brief Amount of allocated memory
     */
    size_t used;

    /**
     * @brief Amount of free (unallocated) memory
     */
    size_t free;
};

typedef struct allocator_api {
    /**
     * @brief Allocate memory
     *
     * @note The function should never be called with size 0
     *
     * @param void* Pointer to allocator
     * @param size Size of memory to allocate in bytes (should never be 0)
     * @param align Alignment of memory
     * @return void* Pointer to allocated memory
     */
    void *(*alloc)(void *a, size_t size, uint8_t align);

    /**
     * @brief Free memory
     *
     * @param void* Pointer to allocator
     * @param ptr Pointer to memory to free
     */
    int8_t (*free)(void *a, void *ptr);

    /**
     * @param void* Pointer to allocator
     * @brief Reset allocator
     */
    int8_t (*reset)(void *a);

    /**
     * @brief Get allocator statistics
     *
     * @param void* Pointer to allocator
     * @param stats Pointer to statistics structure
     */
    int8_t (*stats)(void *a, struct alloc_stats *stats);
} allocator_api_t;

#ifdef __cplusplus
}
#endif

#endif