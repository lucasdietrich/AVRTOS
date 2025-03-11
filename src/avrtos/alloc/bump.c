/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <avrtos/defines.h>
#include <avrtos/errno.h>
#include <avrtos/assert.h>

#include "alloc_private.h"
#include "bump.h"

#define K_MODULE K_MODULE_ALLOC

int8_t k_bump_init(struct k_bump_allocator *a, uint8_t *buf, size_t size)
{
	__ASSERT_NOTNULL(a);
	__ASSERT_NOTNULL(buf);
	__ASSERT_TRUE(size > 0);
	
	a->buf = buf;
	a->size = size;
	a->next = buf;

	return 0;
}

void *k_bump_alloc(struct k_bump_allocator *a, size_t size, uint8_t align)
{
	__ASSERT_NOTNULL(a);
	__ASSERT_TRUE((size > 0) && (align > 0));

	uint8_t *const ptr = ALIGN_PTR(a->next, align);

	if (ptr - a->buf + size > a->size) {
		return NULL;
	}

	a->next = ptr + size;

	return ptr;
}

void k_bump_reset(struct k_bump_allocator *a)
{
	__ASSERT_NOTNULL(a);

	a->next = a->buf;
}

void k_bump_stats(struct k_bump_allocator *a, struct k_alloc_stats *stats)
{
	__ASSERT_NOTNULL(a);
	__ASSERT_NOTNULL(stats);

	stats->total = a->size;
	stats->used	 = a->next - a->buf;
	stats->free	 = a->size - stats->used;
}