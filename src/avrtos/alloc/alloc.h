/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 /**
 * This header provides EXPERIMENTAL memory allocation functions, including standard 
 * and aligned memory allocation. The current default allocator is a BUMP ALLOCATOR, 
 * which means:
 * 
 *  - Memory is allocated sequentially from a preallocated buffer.
 *  - Individual memory blocks cannot be freed; only a full reset is possible.
 *  - Allocation is fast (O(1)) !
 *
 * @warning **Experimental Feature:** The bump allocator is currently the default 
 * allocator. It does not support freeing 
 * individual allocations (`k_free` is a no-op in this implementation).
 *
 * ## Memory Space and Configuration
 *
 * The current *heap*  operates within its own dedicated memory region, 
 * i.e. it does not overlap with any other memory region (e.g., thread stacks).
 *
 * @note If precise memory management is required (e.g., frequent allocations and 
 * deallocations), consider implementing a more dynamic allocator or manually managing 
 * memory blocks.
 */

#ifndef _AVRTOS_ALLOC_H
#define _AVRTOS_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Allocate memory using default allocator
 *
 * @param size Size of memory to allocate
 * @return void* Pointer to allocated memory
 * @return NULL on error
 */
void *k_malloc(size_t size);

/**
 * @brief Free memory using default allocator
 *
 * @param ptr Pointer to memory to free
 */
void k_free(void *ptr);

/**
 * @brief Allocate aligned memory using default allocator
 *
 * @param size Size of memory to allocate
 * @param align Alignment of memory
 *  - 0: No alignment
 *  - 1: Align to 2 bytes
 *  - 2: Align to 4 bytes
 *  - 3: Align to 8 bytes
 * @return void* Pointer to allocated memory
 * @return NULL on error
 */
void *z_malloc(size_t size, uint8_t align);

/**
 * @brief Reset the default allocator
 * 
 * @note This function should be used with caution !
 *
 * This function resets the default allocator, freeing all memory.
 */
void z_global_allocator_reset(void);

/**
 * @brief Get statistics of the default allocator
 *
 * @param stats Pointer to a structure to store statistics
 */
void k_global_allocator_stats_get(size_t *total, size_t *used, size_t *free);

#ifdef __cplusplus
}
#endif

#endif