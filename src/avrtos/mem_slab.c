/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mem_slab.h"

#include <avrtos/dstruct/queue.h>
#include <avrtos/kernel.h>
#include <avrtos/kernel_internals.h>

#define K_MODULE K_MODULE_MEMSLAB

static void create_free_list(struct k_mem_slab *slab)
{
	slab->free_list = NULL;
	uint8_t *p	= slab->buffer;

	for (uint8_t i = 0u; i < slab->count; i++) {
		/* equivalent to "*(void**) p = slab->free_list;" */
		((struct qitem *)p)->next = slab->free_list;

		slab->free_list = (struct qitem *)p;
		p += slab->block_size;
	}
}

extern struct k_mem_slab __k_mem_slabs_start;
extern struct k_mem_slab __k_mem_slabs_end;

/* TODO Set always inline */
void z_mem_slab_init_module(void)
{
	/* must be called during MCU initialization, in order to initialize
	 * known memory slabs using the linker section defined
	 */
	const uint8_t mem_slabs_count = &__k_mem_slabs_end - &__k_mem_slabs_start;
	for (uint8_t i = 0; i < mem_slabs_count; i++) {
		create_free_list(&(&__k_mem_slabs_start)[i]);
	}
}

int8_t k_mem_slab_init(struct k_mem_slab *slab,
		       void *buffer,
		       size_t block_size,
		       uint8_t num_blocks)
{
	if (buffer == NULL) {
		return -EINVAL;
	}

	/* we need a list 2 bytes to store the next
	 * free block address if not allocated
	 */
	if ((block_size < 2) || (num_blocks == 0)) {
		return -EINVAL;
	}

	slab->block_size = block_size;
	slab->count	 = num_blocks;
	slab->buffer	 = buffer;
	create_free_list(slab);

	dlist_init(&slab->waitqueue);

	return 0;
}

/**
 * @brief Internal function to allocate a block
 *
 * Assumptions :
 * - slab not null
 * - mem not null
 * - interrupts are disabled
 *
 * @param slab
 * @param mem
 * @return K_NOINLINE
 */
__always_inline static int8_t z_mem_slab_alloc(struct k_mem_slab *slab, void **mem)
{
	*mem		= (uint8_t *)slab->free_list;
	slab->free_list = slab->free_list->next;

	return 0;
}

int8_t k_mem_slab_alloc(struct k_mem_slab *slab, void **mem, k_timeout_t timeout)
{
	__ASSERT_NOTNULL(slab);
	__ASSERT_NOTNULL(mem);

	int8_t ret;

	const uint8_t key = irq_lock();

	if (slab->free_list != NULL) {
		/* there are fre memory blocks,
		 * so we allocate one directly */
		ret = z_mem_slab_alloc(slab, mem);
	} else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		/* we don't wait for a block to available and there no
		 * one left, we return an error */
		ret = -ENOMEM;
	} else {
		/* we wait for a block being available */
		ret = z_pend_current(&slab->waitqueue, timeout);
		if (ret == 0) {
			/* we retrieve the memory block available */
			*mem = z_current->swap_data;
		}
	}

	irq_unlock(key);

	return ret;
}

/**
 * @brief Internal function to free a block
 *
 * Assumptions :
 * - slab not null
 * - mem not null
 * - interrupts are disabled
 * - mem is currently "allocated"
 *
 * @param slab
 * @param mem
 * @return K_NOINLINE
 */
K_NOINLINE static int8_t z_mem_slab_free(struct k_mem_slab *slab, void *mem)
{
	__ASSERT_NOTNULL(slab);
	__ASSERT_NOTNULL(mem);

	/* eq to "**(struct qitem***)mem = slab->free_list;" */

	((struct qitem *)mem)->next = slab->free_list;
	slab->free_list		    = (struct qitem *)mem;

	return 0;
}

struct k_thread *k_mem_slab_free(struct k_mem_slab *slab, void *mem)
{
	__ASSERT_NOTNULL(slab);

	struct k_thread *thread = NULL;

	if (mem == NULL) {
		goto ret;
	}

	const uint8_t key = irq_lock();

	/* if a thread is pending on a memory slab we give the block
	 * directly to the thread (using thread->swap_data)
	 */
	thread = z_unpend_first_and_swap(&slab->waitqueue, mem);

	if (thread == NULL) {
		/* otherwise we free the block */
		z_mem_slab_free(slab, mem);
	}

	irq_unlock(key);

ret:
	return thread;
}
