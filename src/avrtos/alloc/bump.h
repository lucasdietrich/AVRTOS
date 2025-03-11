/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 /*
 * Bump Allocator
 *
 * A bump allocator is a simple memory management mechanism that sequentially allocates
 * memory from a preallocated buffer. It is extremely efficient, providing O(1) allocation
 * time, but does not support deallocation.
 * 
 * Deallocation is only possible by resetting the whole allocator, effectively freeing all
 * allocated memory.
 * 
 * The current bump allocator API is NOT thread-safe.
 *
 * The allocator maintains a pointer (`next`) that tracks the next free memory location.
 * When memory is requested, the `next` pointer is advanced by the requested size,
 * ensuring alignment as necessary. If there is not enough space left in the buffer,
 * allocation fails and returns NULL.
 *
 * Example with a bump allocator managing a 128-byte buffer:
 *
 *  | Allocated        | Allocated        | Free Space
 *  | 0x0000           | 0x0020           | 0x0040
 *  +------------------+------------------+------------------+------------------+
 *  | 32-byte block    | 32-byte block    | 64 bytes remain                     |
 *  +------------------+------------------+------------------+------------------+
 *                                         ^
 *                                         |
 *                              Next allocation starts here
 */

#ifndef _AVRTOS_ALLOC_BUMP_H
#define _AVRTOS_ALLOC_BUMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include <avrtos/alloc/api.h>
#include <avrtos/alloc/alloc_private.h>

/**
 * @brief Bump allocator structure
 */
struct k_bump_allocator {
	/**
	 * @brief Pointer to memory buffer
	 */
	uint8_t *buf;

	/**
	 * @brief Size of memory buffer
	 */
	size_t size;

	/**
	 * @brief Next free memory location
	 */
	uint8_t *next;
};

/**
 * @brief Define and initialize buffer and bump allocator at compile time
 *
 * @param name Name of bump allocator
 * @param size Size of memory buffer
 */
#define K_BUMP_ALLOC_DEFINE(name, size)                                                  \
	uint8_t name##_buf[size];                                                            \
	struct k_bump_allocator name = K_BUMP_ALLOC_INIT(name##_buf, size)

/**
 * @brief Initialize bump allocator at compile time
 *
 * @param _buf Pointer to memory buffer
 * @param _size Size of memory buffer
 */
#define K_BUMP_ALLOC_INIT(_buf, _size)                                                     \
	{                                                                                    \
		.buf = _buf, .size = _size, .next = _buf,                                           \
	}

/**
 * @brief Initialize bump allocator at runtime
 *
 * @param a Pointer to bump allocator structure
 * @param buf Pointer to memory buffer
 * @param size Size of memory buffer
 * @return int8_t 0 on success, -1 on error
 */
int8_t k_bump_init(struct k_bump_allocator *a, uint8_t *buf, size_t size);

/**
 * @brief Allocate memory from bump allocator
 *
 * @param a Pointer to bump allocator structure
 * @param size Size of memory to allocate
 * @param align Alignment of memory
 * @return void* Pointer to allocated memory, NULL on error
 */
void *k_bump_alloc(struct k_bump_allocator *a, size_t size, uint8_t align);

/**
 * @brief Reset bump allocator
 *
 * @param a Pointer to bump allocator structure
 */
void k_bump_reset(struct k_bump_allocator *a);

/**
 * @brief Get bump allocator statistics
 *
 * @param a Pointer to bump allocator structure
 * @param stats Pointer to statistics structure
 */
void k_bump_stats(struct k_bump_allocator *a, struct k_alloc_stats *stats);

#ifdef __cplusplus
}
#endif

#endif