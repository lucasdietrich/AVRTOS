/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Slab Allocator
 *
 * A memory slab is a memory management mechanism that provides a pool of fixed-size
 * memory blocks. Each slab is initialized with a predefined number of blocks, all of
 * which are of the same size. Memory slabs provides deterministic allocation and
 * deallocation times (O(1)).
 *
 * Example with a memory slab containing 4 blocks of 32 bytes each:
 *
 *  | Block 1          | Block 2          | Block 3          | Block 4
 *  | 0x0000           | 0x0020           | 0x0040           | 0x0060
 *  +------------------+------------------+------------------+------------------+
 *  | Free Block       | Free Block       | Allocated Block  | Allocated Block  |
 *  |                  | Next Free Block  |                  |                  |
 *  +------------------+------------------+------------------+------------------+
 *                             |                    |                   |
 *                             |                    |                   |
 *                             v                    v                   v
 *                        Next block     Second allocated block  First allocated block
 *                        in free list          is here              is here
 */

#ifndef _AVRTOS_ALLOC_SLAB_H
#define _AVRTOS_ALLOC_SLAB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include <avrtos/alloc/alloc_private.h>
#include <avrtos/alloc/api.h>

/**
 * @brief Memory Slab Structure
 *
 * A memory slab is a pool of fixed-size memory blocks that can be allocated
 * and freed. The memory blocks are managed using a singly linked list that
 * tracks the free blocks.
 *
 * Allocation and deallocation operations are performed in constant time (O(1)).
 *
 * @note Maximum number of blocks is 255.
 * @note Maximum block size is 65535 bytes
 */
struct k_slab_allocator {
	void *buffer;			 /**< Pointer to the memory buffer */
	uint8_t count;			 /**< Total number of blocks in the slab */
	uint16_t block_size;	 /**< Size of each block in bytes */
	struct snode *free_list; /**< Pointer to the list of free blocks */
};

/**
 * @brief Initialize slab allocator at runtime
 *
 * @param a Pointer to slab allocator structure
 * @param buffer Pointer to memory buffer
 * @param block_size Size of memory block
 * @param blocks_count Number of memory blocks
 * @return int8_t 0 on success, or an error code on failure
 */
int8_t k_slab_init(struct k_slab_allocator *a,
				   void *buffer,
				   size_t block_size,
				   uint8_t blocks_count);

/**
 * @brief Allocate a slab
 *
 * @param a Pointer to slab allocator structure
 * @return void* Pointer to allocated slab, NULL on error
 */
void *k_slab_alloc(struct k_slab_allocator *a);

/**
 * @brief Free a slab
 *
 * @param a Pointer to slab allocator structure
 * @param ptr Pointer to slab to free
 */
void k_slab_free(struct k_slab_allocator *a, void *ptr);

/**
 * @brief Reset slab allocator
 *
 * @param a Pointer to slab allocator structure
 */
void k_slab_reset(struct k_slab_allocator *a);

/**
 * @brief Get slab allocator statistics
 *
 * @param a Pointer to slab allocator structure
 * @param stats Pointer to statistics structure
 */
void k_slab_stats(struct k_slab_allocator *a, struct k_alloc_stats *stats);

#ifdef __cplusplus
}
#endif

#endif