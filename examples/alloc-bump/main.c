/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/alloc/alloc.h>
#include <avrtos/alloc/bump.h>
#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>

#include <avr/io.h>

struct bump_allocator bump;

uint8_t buf[256u];

int main(void)
{
	int8_t ret = 0;

	ret = bump_init(&bump, buf, sizeof(buf));
	printf("bump_init (buf: %p): %d\n", buf, ret);

	void *ptr = bump_alloc(&bump, 16, Z_NO_ALIGN);
	printf("bump_alloc: %p\n", ptr);

	ptr = bump_alloc(&bump, 1, Z_NO_ALIGN);
	printf("bump_alloc: %p\n", ptr);

	ptr = bump_alloc(&bump, 16, Z_ALIGN_2);
	printf("bump_alloc: %p\n", ptr);

	ptr = bump_alloc(&bump, 1, Z_NO_ALIGN);
	printf("bump_alloc: %p\n", ptr);

	ptr = bump_alloc(&bump, 16, Z_ALIGN_4);
	printf("bump_alloc: %p\n", ptr);

	ptr = bump_alloc(&bump, 16, Z_ALIGN_8);
	printf("bump_alloc: %p\n", ptr);

	ptr = bump_alloc(&bump, 16, Z_ALIGN_16);
	printf("bump_alloc: %p\n", ptr);

	struct alloc_stats stats;
	bump_stats(&bump, &stats);

	printf("total: %u used: %u free: %u\n", stats.total, stats.used, stats.free);

	ptr = bump_alloc(&bump, stats.free, Z_NO_ALIGN);
	printf("bump_alloc: %p\n", ptr);

	ptr = bump_alloc(&bump, 1, Z_NO_ALIGN);
	printf("bump_alloc: %p\n", ptr);
}