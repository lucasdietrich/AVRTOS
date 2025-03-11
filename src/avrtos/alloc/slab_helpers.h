/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_ALLOC_SLAB_PRIVATE_H
#define _AVRTOS_ALLOC_SLAB_PRIVATE_H

#include "slab.h"

/**
 * @brief Statically initialize a memory slab allocator.
 *
 * This macro initializes a memory slab structure at compile time.
 *
 * @param _slab The memory slab structure to initialize.
 * @param _buf The buffer used by the memory slab.
 * @param _block_size Size of each block in the memory slab.
 * @param _blocks_count Total number of blocks in the memory slab.
 */
#define Z_SLAB_INIT(_slab, _buf, _block_size, _blocks_count)                             \
	{                                                                                    \
		.buffer = _buf, .count = _blocks_count, .block_size = _block_size,               \
		.free_list = NULL                                                                \
	}

/**
 * @brief Macro to generate a unique name for the memory slab allocator.
 *
 * This macro generates a unique buffer name for the memory slab.
 *
 * @param _slab_name The name of the memory slab.
 */
#define Z_SLAB_BUF_NAME(_slab_name) z_slab_buf_##_slab_name

/**
 * @brief Statically define and initialize a memory slab buffer and allocator in a
 * specific section.
 *
 * This macro defines a memory slab and its associated buffer, initializing
 * both at compile time and places them in a specific section.
 *
 * @param _slab_name Name of the memory slab.
 * @param _block_size Size of each block in the memory slab.
 * @param _blocks_count Total number of blocks in the memory slab.
 * @param _section_name Name of the section to place the slab in.
 */
#define Z_SLAB_DEFINE(_slab_name, _block_size, _blocks_count, _section_name)             \
	uint8_t Z_SLAB_BUF_NAME(_slab_name)[(_block_size) * (_blocks_count)];                \
	Z_LINK_KERNEL_SECTION(_section_name)                                                 \
	static struct k_mem_slab _slab_name =                                                \
		Z_SLAB_INIT(_slab_name, Z_SLAB_BUF_NAME(_slab_name), _block_size, _blocks_count)

#endif