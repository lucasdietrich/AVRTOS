/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** Default allocator */

#include <avrtos/kernel.h>

#include "alloc.h"
#include "alloc_private.h"
#include "bump.h"

#define K_MODULE K_MODULE_ALLOC

#if CONFIG_KERNEL_GLOBAL_ALLOCATOR_SIZE > 0
#warning "Global allocator is enabled and uses the bump allocator implementation"

/* Current global allocator relies on the bump allocator, which is totally
 * experimental !
 */
BUMP_ALLOC_DEFINE(z_global_allocator, CONFIG_KERNEL_GLOBAL_ALLOCATOR_SIZE);

void *z_malloc(size_t size, uint8_t align)
{
	/* Underlying allocator does not support 0-size allocations */
	if (size == 0) {
		return NULL;
	}

	const uint8_t key = irq_lock();

	void *ptr = bump_alloc(&z_global_allocator, size, align);

	irq_unlock(key);

	return ptr;
}

void *k_malloc(size_t size)
{
	// Align does not matter for data
	return z_malloc(size, Z_NO_ALIGN);
}

void k_free(void *ptr)
{
	(void)ptr;
}

void z_global_allocator_reset(void)
{
	const uint8_t key = irq_lock();

	bump_reset(&z_global_allocator);

	irq_unlock(key);
}

void k_global_allocator_stats_get(size_t *total, size_t *used, size_t *free)
{
	const uint8_t key = irq_lock();

	struct alloc_stats stats;
	bump_stats(&z_global_allocator, &stats);

	irq_unlock(key);

	*total = stats.total;
	*used  = stats.used;
	*free  = stats.free;
}
#endif