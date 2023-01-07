#include "ring.h"

int k_ring_init(struct k_ring *ring, uint8_t *buffer, uint8_t size)
{
	ring->buffer = buffer;
	ring->size   = size;
	ring->r	     = 0u;
	ring->w	     = 0u;

	return 0;
}

int k_ring_push(struct k_ring *ring, char data)
{
	const uint8_t r = ring->r;
	uint8_t w	= ring->w + 1u;

	if (w == r) {
		return -EAGAIN;
	} else if (w == ring->size) {
		w = 0u;
	}

	ring->buffer[w] = data;
	ring->w		= w;

	return 0;
}

int k_ring_pop(struct k_ring *ring)
{
	const uint8_t w = ring->w;
	uint8_t r	= ring->r;

	if (r == w) {
		return -EAGAIN;
	}

	const char data = ring->buffer[r];

	r++;
	if (r == ring->size) {
		r = 0u;
	}

	ring->r = r;

	return data;
}

int k_ring_reset(struct k_ring *ring)
{
	return 0;
}