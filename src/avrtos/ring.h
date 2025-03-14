/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Naive ring buffer
 *
 * A ring buffer is a fixed-size buffer that wraps around when the end of the buffer is
 * reached.
 *
 * Limitations:
 * - This implementation is currently not thread/interrupt-safe. For concurrent
 * environments, proper synchronization mechanisms must be implemented.
 *
 * TODOs:
 * - Introduce support for concurrent writers and readers:
 * - Optimize for power-of-two buffer sizes
 */

#ifndef _AVRTOS_RING_H
#define _AVRTOS_RING_H

#include "kernel.h"

/**
 * @brief Structure representing a ring buffer.
 *
 * The ring buffer structure contains pointers to the buffer, along with read and write
 * cursors to manage data access. The size of the buffer is also stored to control the
 * buffer's wrap-around behavior.
 */
struct k_ring {
    uint8_t *buffer; /**< Pointer to the buffer array used by the ring buffer */

    uint8_t r; /**< Read cursor, indicating the position of the next byte to read */

    uint8_t w; /**< Write cursor, indicating the position of the next byte to write */

    uint8_t size; /**< Size of the buffer (total number of elements it can hold) */
};

/**
 * @brief Macro to initialize a ring buffer.
 *
 * This macro initializes a ring buffer structure with the provided buffer and size.
 *
 * @param _buf Pointer to the buffer array.
 * @param _size Size of the buffer array.
 */
#define Z_RING_INIT(_buf, _size)                                                         \
    {                                                                                    \
        .buffer = _buf, .r = 0u, .w = 0u, .size = _size,                                 \
    }

/**
 * @brief Macro to define and initialize a ring buffer.
 *
 * This macro defines a buffer array and a ring buffer structure, and initializes the
 * structure with the buffer and size.
 *
 * @param _name Name of the ring buffer instance.
 * @param _size Size of the buffer array.
 */
#define K_RING_DEFINE(_name, _size)                                                      \
    uint8_t _name##_buf[_size];                                                          \
    struct k_ring _name = Z_RING_INIT(_name##_buf, _size)

/**
 * @brief Initialize a ring buffer with a given buffer and size.
 *
 * This function initializes the ring buffer structure with the provided buffer and size.
 *
 * @param ring Pointer to the ring buffer structure to initialize.
 * @param buffer Pointer to the buffer array to be used by the ring buffer.
 * @param size Size of the buffer array.
 * @return int8_t Returns 0 on success
 * @return -EINVAL if the ring pointer or buffer pointer is NULL.
 */
int8_t k_ring_init(struct k_ring *ring, uint8_t *buffer, uint8_t size);

/**
 * @brief Push a byte of data into the ring buffer.
 *
 * This function adds a byte of data to the ring buffer at the current write cursor
 * position and advances the write cursor. If the buffer is full, the function will
 * overwrite the oldest data.
 *
 * @param ring Pointer to the ring buffer structure.
 * @param data Byte of data to push into the buffer.
 * @return int8_t Returns 0 on success
 * @return -EINVAL if the ring pointer is NULL.
 * @return -ENOMEM if the buffer is full.
 */
int8_t k_ring_push(struct k_ring *ring, char data);

/**
 * @brief Pop a byte of data from the ring buffer.
 *
 * This function retrieves a byte of data from the ring buffer at the current read cursor
 * position and advances the read cursor. If the buffer is empty, the function will return
 * an error.
 *
 * @param ring Pointer to the ring buffer structure.
 * @param data Pointer to the variable where the popped data will be stored.
 * @return int8_t Returns 0 on success
 * @return -EINVAL if the ring pointer or data pointer is NULL.
 * @return -EAGAIN if the buffer is empty.
 */
int8_t k_ring_pop(struct k_ring *ring, char *data);

/**
 * @brief Reset the ring buffer.
 *
 * This function resets the ring buffer, clearing any data and resetting the read and
 * write cursors to their initial positions.
 *
 * @param ring Pointer to the ring buffer structure.
 * @return int8_t Returns 0 on success, or a negative error code on failure.
 */
int8_t k_ring_reset(struct k_ring *ring);

#endif /* _AVRTOS_RING_H */
