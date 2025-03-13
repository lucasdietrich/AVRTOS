/*
 * Copyright (c) 2022-2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Memory Slabs
 *
 * A memory slab is a memory management mechanism that provides a pool of fixed-size
 * memory blocks. Refer to the slab allocator <avrtos/alloc/slab.h> for more information.
 * 
 * Current API enables synchronization over memory slab allocator. Threads can allocate
 * blocks from the slab when available, and free them when no longer needed. If no blocks
 * are available, threads can block until a block becomes free.
 *
 * Related configuration options:
 *  - CONFIG_KERNEL_ARGS_CHECKS: Enable argument checks
 *  - CONFIG_AVRTOS_LINKER_SCRIPT: Enables the use of the linker script
 */

#ifndef _AVRTOS_MEM_SLAB_H_
#define _AVRTOS_MEM_SLAB_H_

#include <stdbool.h>
#include <stdint.h>

#include "kernel.h"
#include "alloc/slab.h"
#include "alloc/slab_private.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Memory Slab Structure
 *
 * Memory slab subsystem enables synchronization over memory slab allocator.
 */
struct k_mem_slab {
	struct k_slab_allocator allocator; /**< Slab allocator */
	struct dnode waitqueue;	 /**< Wait queue for threads pending on a block */
};

/**
 * @brief Statically initialize a memory slab.
 *
 * This macro initializes a memory slab structure at compile time.
 *
 * @param _slab The memory slab structure to initialize.
 * @param _buf The buffer used by the memory slab.
 * @param _block_size Size of each block in the memory slab.
 * @param _blocks_count Total number of blocks in the memory slab.
 */
#define Z_MEM_SLAB_INIT(_slab, _buf, _block_size, _blocks_count)                         \
	{                                                                                    \
		.allocator = Z_SLAB_INIT(_slab, _buf, _block_size, _blocks_count),               \
		.waitqueue = DLIST_INIT(_slab.waitqueue)                                         \
	}

/**
 * @brief Macro to generate a unique name for the memory slab buffer.
 *
 * This macro generates a unique buffer name for the memory slab.
 *
 * @param _slab_name The name of the memory slab.
 */
#define Z_MEM_SLAB_BUF_NAME(_slab_name) z_mem_slab_buf_##_slab_name

/**
 * @brief Statically define and initialize a memory slab.
 *
 * This macro defines a memory slab and its associated buffer, initializing
 * both at compile time.
 *
 * @param _slab_name Name of the memory slab.
 * @param _block_size Size of each block in the memory slab.
 * @param _blocks_count Total number of blocks in the memory slab.
 */
#define K_MEM_SLAB_DEFINE(_slab_name, _block_size, _blocks_count)                        \
	uint8_t Z_MEM_SLAB_BUF_NAME(_slab_name)[(_block_size) * (_blocks_count)];            \
	Z_LINK_KERNEL_SECTION(.k_mem_slabs)                                                  \
	static struct k_mem_slab _slab_name = Z_MEM_SLAB_INIT(                               \
		_slab_name, Z_MEM_SLAB_BUF_NAME(_slab_name), _block_size, _blocks_count)

/**
 * @brief Initialize all statically defined memory slabs.
 *
 * This function is called at startup to initialize all memory slabs that
 * were defined using the K_MEM_SLAB_DEFINE macro.
 */
__kernel void z_mem_slab_init_module(void);

/**
 * @brief Initialize a memory slab at runtime.
 *
 * This function initializes a memory slab with the specified buffer, block size,
 * and number of blocks.
 *
 * @param slab Pointer to the memory slab structure.
 * @param buffer Pointer to the buffer used by the memory slab.
 * @param block_size Size of each block in the memory slab.
 * @param blocks_count Total number of blocks in the memory slab.
 * @return 0 on success, or an error code on failure.
 */
__kernel int8_t k_mem_slab_init(struct k_mem_slab *slab,
								void *buffer,
								size_t block_size,
								uint8_t blocks_count);

/**
 * @brief Allocate a memory block from a slab.
 *
 * This function allocates a memory block from the specified slab. If no block
 * is available and a timeout is provided, the function will block until a block
 * becomes available or the timeout expires.
 *
 * Safety: This function is generally not safe to call from an ISR context
 *         if the timeout is different from K_NO_WAIT.
 *
 * @param slab Pointer to the memory slab structure.
 * @param mem Pointer to the variable that will receive the allocated memory block.
 * @param timeout Maximum time to wait for a block to become available.
 * @return 0 on success, or an error code on failure:
 *         - ENOMEM: No block available.
 *         - ETIMEDOUT: Timeout expired.
 *         - ECANCELED: Wait aborted.
 */
__kernel int8_t k_mem_slab_alloc(struct k_mem_slab *slab,
								 void **mem,
								 k_timeout_t timeout);

/**
 * @brief Free a memory block back to a slab.
 *
 * This function returns a memory block to the specified slab and notifies the first
 * thread in the wait queue that a block has become available.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param slab Pointer to the memory slab structure.
 * @param mem Pointer to the memory block being freed.
 * @return The thread that was woken up, or NULL if no threads were waiting.
 */
__kernel struct k_thread *k_mem_slab_free(struct k_mem_slab *slab, void *mem);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_MEM_SLAB_H_ */
