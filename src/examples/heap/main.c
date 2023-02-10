/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/kernel.h>
#include <avrtos/logging.h>
#define LOG_LEVEL LOG_LEVEL_DEBUG

struct k_heap heap;

char buf[0x100];

struct block {
	struct snode tie;
};

SLIST_DEFINE(ablocks);

int main(void)
{
	struct block *ptr;

	serial_init();

	k_heap_init(&heap, buf, sizeof(buf));

	do {
		ptr = k_heap_alloc(&heap, 16u);
		if (ptr) {
			slist_append(&ablocks, &ptr->tie);
			LOG_DBG("ALLOC ptr: %u", sys_ptr_diff(ptr, buf));
		} else {
			LOG_ERR("Out of memory");
		}
	} while (ptr);

	while ((ptr = (struct block *)slist_get(&ablocks)) != NULL) {
		LOG_DBG("FREE: ptr: %u", sys_ptr_diff(ptr, buf));
		k_heap_free(&heap, ptr);
	}

	for (;;) {
	}
}