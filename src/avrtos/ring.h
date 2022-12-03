/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_RING_H
#define _AVRTOS_RING_H

/* Implement an atomic thread/irq-safe ring buffer */

#include <avrtos/kernel.h>

#define K_RING_CONCURRENT_WRITTERS 0u
#define K_RING_CONCURRENT_READERS 0u
// #define K_RING_SIZE_POWER_OF_TWO 0u

struct k_ring
{
	uint8_t *buffer;

	/* Read cursor */
	uint8_t r;

	/* Write cursor */
	uint8_t w;

	/* Contain size of the ring buffer */
	uint8_t size;
};

int k_ring_init(struct k_ring *ring, uint8_t *buffer, uint8_t size);

int k_ring_push(struct k_ring *ring, char data);

int k_ring_pop(struct k_ring *ring);

int k_ring_reset(struct k_ring *ring);

#endif /* _AVRTOS_RING_H */