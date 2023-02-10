/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_HEAP_H_
#define _AVRTOS_HEAP_H_

#include <stdbool.h>
#include <stdint.h>

#include <avrtos/avrtos.h>

struct block_header;

struct block_header {
	struct block_header *next;
	size_t size;
	char data[];
};

struct k_heap {
	struct block_header *free_list;
};

void k_heap_init(struct k_heap *heap, void *buf, size_t size);

void *k_heap_alloc(struct k_heap *heap, size_t alloc_size);

void k_heap_free(struct k_heap *heap, void *ptr);

#endif /* _AVRTOS_HEAP_H_ */