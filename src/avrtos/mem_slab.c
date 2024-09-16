/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mem_slab.h"

#include "dstruct/dlist.h"
#include "dstruct/slist.h"
#include "kernel.h"
#include "kernel_private.h"

#define K_MODULE K_MODULE_MEMSLAB

static void z_create_free_list(struct k_mem_slab *slab)
{
	slab->free_list = NULL;
	uint8_t *p		= slab->buffer;

	for (uint8_t i = 0u; i < slab->count; i++) {
		/* Link the current block to the next free block in the list */
		((struct snode *)p)->next = slab->free_list;

		/* Update the head of the free list to the current block */
		slab->free_list = (struct snode *)p;
		p += slab->block_size; /* Move to the next block */
	}
}

#if CONFIG_AVRTOS_LINKER_SCRIPT
extern struct k_mem_slab __k_mem_slabs_start;
extern struct k_mem_slab __k_mem_slabs_end;

/**
 * This function is called during avrtos initialization to initialize all known memory
 * slabs that are defined using the linker section.
 */
void z_mem_slab_init_module(void)
{
	/* Calculate the number of memory slabs defined in the linker script */
	const uint8_t mem_slabs_count = &__k_mem_slabs_end - &__k_mem_slabs_start;
	for (uint8_t i = 0; i < mem_slabs_count; i++) {
		/* Initialize each slab's free list */
		z_create_free_list(&(&__k_mem_slabs_start)[i]);
	}
}
#else
/**
 * This function is used to complete the initialization of a memory slab when it
 * is declared without the linker script.
 */
__kernel void z_mem_slab_finalize_init(struct k_mem_slab *slab)
{
	/* Re-create the free list for the slab */
	z_create_free_list(slab);
}
#endif

int8_t k_mem_slab_init(struct k_mem_slab *slab,
					   void *buffer,
					   size_t block_size,
					   uint8_t num_blocks)
{
	/* Validate the arguments: buffer must be non-null, block size must be at least 2
	 * bytes, and there must be at least one block.
	 */
	Z_ARGS_CHECK(buffer && (block_size >= 2) && num_blocks) return -EINVAL;

	/* Initialize the wait queue for threads waiting on this slab */
	dlist_init(&slab->waitqueue);

	/* Set the slab properties */
	slab->block_size = block_size;
	slab->count		 = num_blocks;
	slab->buffer	 = buffer;

	/* Create the free list for the slab */
	z_create_free_list(slab);

	return 0;
}

__always_inline int8_t z_mem_slab_alloc(struct k_mem_slab *slab, void **mem)
{
	/* Allocate a block from the free list */
	*mem			= (uint8_t *)slab->free_list;
	slab->free_list = slab->free_list->next;

	return 0;
}

int8_t k_mem_slab_alloc(struct k_mem_slab *slab, void **mem, k_timeout_t timeout)
{
	Z_ARGS_CHECK(slab && mem) return -EINVAL;

	int8_t ret;

	const uint8_t key = irq_lock();

	if (slab->free_list != NULL) {
		/* There are free memory blocks, allocate one directly */
		ret = z_mem_slab_alloc(slab, mem);
	} else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		/* No blocks available and the thread doesn't want to wait */
		ret = -ENOMEM;
	} else {
		/* Wait for a block to become available */
		ret = z_pend_current(&slab->waitqueue, timeout);
		if (ret == 0) {
			/* Retrieve the memory block available */
			*mem = z_current->swap_data;
		}
	}

	irq_unlock(key);

	return ret;
}

__kernel static int8_t z_mem_slab_free(struct k_mem_slab *slab, void *mem)
{
	__ASSERT_NOTNULL(slab);
	__ASSERT_NOTNULL(mem);

	/* Add the block back to the free list */
	((struct snode *)mem)->next = slab->free_list;
	slab->free_list				= (struct snode *)mem;

	return 0;
}

struct k_thread *k_mem_slab_free(struct k_mem_slab *slab, void *mem)
{
	Z_ARGS_CHECK(slab && mem) return NULL;

	struct k_thread *thread = NULL;

	if (mem == NULL) {
		/* If the memory block is NULL, there's nothing to free */
		goto ret;
	}

	const uint8_t key = irq_lock();

	/* If a thread is pending on the memory slab, give the block directly to the thread */
	thread = z_unpend_first_and_swap(&slab->waitqueue, mem);

	if (thread == NULL) {
		/* Otherwise, free the block */
		z_mem_slab_free(slab, mem);
	}

	irq_unlock(key);

ret:
	return thread;
}
