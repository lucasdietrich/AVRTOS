/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>

#include <avrtos/alloc/alloc.h>
#include <avrtos/alloc/bump.h>

#include <avr/io.h>

struct k_bump_allocator bump;

uint8_t buf[256u];

int main(void)
{
	int8_t ret = 0;

	ret = k_bump_init(&bump, buf, sizeof(buf));
	printf("k_bump_init (buf: %p): %d\n", buf, ret);

	void *ptr = k_bump_alloc(&bump, 16, K_NO_ALIGN);
	printf("k_bump_alloc: %p\n", ptr);

	ptr = k_bump_alloc(&bump, 1, K_NO_ALIGN);
	printf("k_bump_alloc: %p\n", ptr);

	ptr = k_bump_alloc(&bump, 16, K_ALIGN_2);
	printf("k_bump_alloc: %p\n", ptr);

	ptr = k_bump_alloc(&bump, 1, K_NO_ALIGN);
	printf("k_bump_alloc: %p\n", ptr);

	ptr = k_bump_alloc(&bump, 16, K_ALIGN_4);
	printf("k_bump_alloc: %p\n", ptr);

	ptr = k_bump_alloc(&bump, 16, K_ALIGN_8);
	printf("k_bump_alloc: %p\n", ptr);

	ptr = k_bump_alloc(&bump, 16, K_ALIGN_16);
	printf("k_bump_alloc: %p\n", ptr);

	struct k_alloc_stats stats;
	k_bump_stats(&bump, &stats);

	printf("total: %u used: %u free: %u\n", stats.total, stats.used, stats.free);

	ptr = k_bump_alloc(&bump, stats.free, K_NO_ALIGN);
	printf("k_bump_alloc: %p\n", ptr);

	ptr = k_bump_alloc(&bump, 1, K_NO_ALIGN);
	printf("k_bump_alloc: %p\n", ptr);
}