#include "ring.h"

int8_t k_ring_init(struct k_ring *ring, uint8_t *buffer, uint8_t size)
{
    if (!z_user(ring && buffer && size))
        return -EINVAL;

    ring->buffer = buffer;
    ring->size   = size;
    ring->r      = 0u;
    ring->w      = 0u;

    return 0;
}

int8_t k_ring_push(struct k_ring *ring, char data)
{
    if (!z_user(ring))
        return -EINVAL;

    uint8_t w         = ring->w;
    const uint8_t rem = ring->size - (w - ring->r) - 1u;

    if (!rem) {
        return -ENOMEM;
    }

    ring->buffer[w] = data;

    /* ensure the data is written before the write cursor is published */
    memory_barrier();

    w++;
    if (w == ring->size)
        w = 0u;

    ring->w = w;

    return 0;
}

int8_t k_ring_pop(struct k_ring *ring, char *data)
{
    if (!z_user(ring && data))
        return -EINVAL;

    uint8_t r       = ring->r;

    if (r == ring->w)
        return -EAGAIN;

    *data = ring->buffer[r];

    /* ensure the data is read before the read cursor (freeing the slot) is published */
    memory_barrier();

    r++;
    if (r == ring->size)
        r = 0u;

    ring->r = r;

    return 0;
}

int8_t k_ring_reset(struct k_ring *ring)
{
    ARG_UNUSED(ring);

    return -ENOTSUP;
}