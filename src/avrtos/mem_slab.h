/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_MEM_SLAB_H
#define _AVRTOS_MEM_SLAB_H

#include <stdbool.h>
#include <stdint.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Implement a pool of memory blocks in RAM.
 *
 * This implementation is greatly inspired from zephyr rtos :
 *  https://github.com/zephyrproject-rtos/zephyr/blob/main/kernel/mem_slab.c
 *
 * Allocation is constant time. We're using a singly-linked list to list all
 * free memory blocks. The list items are stored at the first bytes of free
 * memory blocks :
 * - when a block is allocated, the item is removed from the free_list
 * - when a block is freed, the item is queue to the free_list
 *
 * Complexity :
 * - Initialization is O(n)
 * - Allocation/free is O(1)
 *
 * Maximum number of blocks is 255
 * Maximum block _block_size is 65535 (but technically impossible on AVR
 * architectures)
 */

/**
 * @brief Structure representing a memory slab.
 *
 * buffer : buffer from which all blocks will be allocated.
 * count : numbers of blocks
 * block_size : _block_size of a block
 * free_list : list of all free blocks
 * waitqueue : list contains all threads pending on a fifo item.
 *
 * Note : sizeof(buffer) must equals count * block_size
 */
struct k_mem_slab {
	void *buffer;
	uint8_t count;
	uint16_t block_size;
	struct snode *free_list;
	struct dnode waitqueue;
};

#define K_MEM_SLAB_INIT(_slab, _buf, _block_size, _blocks_count)                   \
	{                                                                          \
		.buffer = _buf, .count = _blocks_count, .block_size = _block_size, \
		.free_list = NULL, .waitqueue = DLIST_INIT(_slab.waitqueue)        \
	}

#define Z_MEM_SLAB_BUF_NAME(_slab_name) z_mem_slab_buf_##_slab_name

#define K_MEM_SLAB_DEFINE(_slab_name, _block_size, _blocks_count)                 \
	uint8_t Z_MEM_SLAB_BUF_NAME(_slab_name)[(_block_size) * (_blocks_count)]; \
	Z_LINK_KERNEL_SECTION(.k_mem_slabs)                                       \
	static struct k_mem_slab _slab_name = K_MEM_SLAB_INIT(                    \
		_slab_name, Z_MEM_SLAB_BUF_NAME(_slab_name), _block_size, _blocks_count)

/**
 * @brief This function is called on start up to initialize all memory
 * slabs defined at runtime using the macro K_MEM_SLAB_DEFINE
 */
__kernel void z_mem_slab_init_module(void);

/**
 * @brief Initialize a memory slab at runtime
 *
 * @param slab address of the slab structure
 * @param buffer address of the buffer
 * @param block_size
 * @param blocks_count
 * @return return 0 on success else error code
 */
__kernel int8_t k_mem_slab_init(struct k_mem_slab *slab,
				void *buffer,
				size_t block_size,
				uint8_t blocks_count);

/**
 * @brief Finalize the initialization of a memory slab when declared using
 * K_MEM_SLAB_DEFINE and CONFIG_AVRTOS_LINKER_SCRIPT is disabled.
 *
 * @param slab
 */
__kernel void z_mem_slab_finalize_init(struct k_mem_slab *slab);

/**
 * @brief Allocate a memory block.
 *
 * If there is no block available timeout is different from K_NO_WAIT,
 * the function is blocking until a block is freed or timeout.
 *
 * Can be called from an interrupt routine with timeout equals to K_NO_WAIT.
 *
 * Assumptions :
 * - slab not null
 * - mem not null
 *
 * @param slab
 * @param mem
 * @param timeout
 * @return 0 on success else error code
 * - ENOMEM : no block available
 * - ETIMEDOUT : timeout expired
 * - EINTR : interrupted by a signal
 * - ECANCELED : wait aborted
 */
__kernel int8_t k_mem_slab_alloc(struct k_mem_slab *slab,
				 void **mem,
				 k_timeout_t timeout);

/**
 * @brief Free a memory block and notify the first pending thread that
 * a mem slab is available.
 *
 * Switch thread before returning if a thread is pending on a block.
 *
 * Can be called from an interrupt routine.
 *
 * Assumptions :
 * - slab not null
 * - mem not null
 * - mem is currently "allocated"
 *
 * @param slab
 * @param mem
 * @return The thread that was woken up or NULL if no thread was woken up.
 */
__kernel struct k_thread *k_mem_slab_free(struct k_mem_slab *slab, void *mem);

#ifdef __cplusplus
}
#endif

#endif