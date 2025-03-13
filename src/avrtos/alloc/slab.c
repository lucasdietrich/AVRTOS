/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include <avrtos/assert.h>
#include <avrtos/defines.h>
#include <avrtos/dstruct/slist.h>
#include <avrtos/errno.h>

#include "alloc_private.h"
#include "slab.h"

#define K_MODULE K_MODULE_ALLOC

/**
 * This function can also be used to complete the initialization of a memory slab when it
 * is partially initialized at compile time.
 */
void z_slab_alloc_finalize_init(struct slab_allocator *a)
{
	a->free_list = NULL;
	uint8_t *p	 = a->buffer;

	for (uint8_t i = 0u; i < a->count; i++) {
		/* Link the current block to the next free block in the list */
		((struct snode *)p)->next = a->free_list;

		/* Update the head of the free list to the current block */
		a->free_list = (struct snode *)p;
		p += a->block_size; /* Move to the next block */
	}
}

int8_t
slab_init(struct slab_allocator *a, void *buffer, size_t block_size, uint8_t blocks_count)
{
	/* Validate the arguments: buffer must be non-null, block size must be at least 2
	 * bytes (to hold at least one pointer), and there must be at least one block.
	 */
	Z_ARGS_CHECK(buffer && (block_size >= 2) && num_blocks) return -EINVAL;

	/* Set the slab properties */
	a->block_size = block_size;
	a->count	  = blocks_count;
	a->buffer	  = buffer;

	/* Create the free list for the slab */
	z_slab_alloc_finalize_init(a);

	return 0;
}

void *slab_alloc(struct slab_allocator *a)
{
	__ASSERT_NOTNULL(a);

	void *mem;

	if (a->free_list == NULL) {
		/* No free memory blocks available */
		mem = NULL;
	} else {
		/* Allocate a block from the free list,
		 * assuming that the free list is not empty.
		 *
		 * Update the free list to point to the next free block.
		 */
		mem			 = (void *)a->free_list;
		a->free_list = a->free_list->next;
	}

	return mem;
}

void slab_free(struct slab_allocator *a, void *ptr)
{
	__ASSERT_NOTNULL(a);
	__ASSERT_NOTNULL(ptr);

	/* Add the block back to the free list */
	((struct snode *)ptr)->next = a->free_list;
	a->free_list				= (struct snode *)ptr;
}

void slab_reset(struct slab_allocator *a)
{
	__ASSERT_NOTNULL(a);

	/* Reset the free list */
	z_slab_alloc_finalize_init(a);
}

void slab_stats(struct slab_allocator *a, struct alloc_stats *stats)
{
	__ASSERT_NOTNULL(a);
	__ASSERT_NOTNULL(stats);

	const uint8_t free_nb = a->free_list ? (slist_count(a->free_list) + 1) : 0;

	stats->total = a->block_size * a->count;
	stats->free	 = free_nb * a->block_size;
	stats->used	 = stats->total - stats->free;
}