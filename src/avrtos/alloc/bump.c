/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include <avrtos/assert.h>
#include <avrtos/defines.h>
#include <avrtos/errno.h>

#include "alloc_private.h"
#include "bump.h"

#define K_MODULE K_MODULE_ALLOC

int8_t bump_init(struct bump_allocator *a, uint8_t *buf, size_t size)
{
    __ASSERT_NOTNULL(a);
    __ASSERT_NOTNULL(buf);
    __ASSERT_TRUE(size > 0);

    a->buf  = buf;
    a->size = size;
    a->next = buf;

    return 0;
}

void *bump_alloc(struct bump_allocator *a, size_t size, uint8_t align)
{
    __ASSERT_NOTNULL(a);
    __ASSERT_TRUE((size > 0) && (align > 0));

    uint8_t *const ptr = Z_ALIGN_PTR(a->next, align);

    if (ptr - a->buf + size > a->size) {
        return NULL;
    }

    a->next = ptr + size;

    return ptr;
}

void bump_reset(struct bump_allocator *a)
{
    __ASSERT_NOTNULL(a);

    a->next = a->buf;
}

void bump_stats(struct bump_allocator *a, struct alloc_stats *stats)
{
    __ASSERT_NOTNULL(a);
    __ASSERT_NOTNULL(stats);

    stats->total = a->size;
    stats->used  = a->next - a->buf;
    stats->free  = a->size - stats->used;
}