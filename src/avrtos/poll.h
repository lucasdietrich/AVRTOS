/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file poll.h
 * @brief Polling API for waiting on multiple kernel objects simultaneously.
 *
 * This header provides the polling API which allows threads to wait for events
 * on multiple kernel objects (semaphores, mutexes, FIFOs, message queues) at the
 * same time. This enables efficient event-driven programming patterns.
 *
 * The polling API is enabled by setting CONFIG_POLLING=1 in the kernel configuration.
 */

#ifndef _AVRTOS_POLL_H
#define _AVRTOS_POLL_H

#include <stdint.h>

#include "avrtos/defines.h"
#include "avrtos/fifo.h"
#include "avrtos/msgq.h"
#include "avrtos/types.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event flag indicating that the polled object is ready.
 */
#define K_POLL_READY 0x01

/**
 * @brief Enumeration of pollable object types.
 *
 * This enum defines the types of kernel objects that can be polled using
 * the k_poll() function.
 */
typedef enum {
    K_POLL_TYPE_SEM   = 0x01, /**< Polling on a semaphore for availability */
    K_POLL_TYPE_MUTEX = 0x02, /**< Polling on a mutex for availability */
    K_POLL_TYPE_MSGQ_PUT =
        0x03, /**< Polling on a message queue for free space to put messages */
    K_POLL_TYPE_MSGQ_GET =
        0x04, /**< Polling on a message queue for available messages to get */
    K_POLL_TYPE_FIFO = 0x05, /**< Polling on a FIFO for available items */
} k_poll_type_t;

/**
 * @brief Structure representing a poll file descriptor.
 *
 * This structure is used to specify which kernel object to poll and what
 * events have occurred on it.
 */
struct k_pollfd {
    z_wqhandle_t _wqhandle;   /**< Internal wait queue handle (private) */
    struct k_thread *_thread; /**< Pointer to the thread that is polling (private) */

    k_poll_type_t type; /**< Type of the object being polled */
    union {
        struct k_sem *sem;     /**< Pointer to a semaphore for polling */
        struct k_mutex *mutex; /**< Pointer to a mutex for polling */
        struct k_fifo *fifo;   /**< Pointer to a FIFO for polling */
        struct k_msgq *msgq;   /**< Pointer to a message queue for polling */
    } obj;                     /**< Union of pointers to the objects being polled */
    uint8_t revents;           /**< Events that have occurred (K_POLL_READY) */
};

/**
 * @brief Poll multiple kernel objects for events.
 *
 * This function allows a thread to wait for events on multiple kernel objects
 * simultaneously. The thread will block until one or more of the specified
 * objects become ready, or until the timeout expires.
 *
 * @param fds Array of poll file descriptors specifying which objects to poll.
 * @param nfds Number of file descriptors in the array.
 * @param timeout Maximum time to wait for events. Use K_FOREVER to wait indefinitely,
 *                or K_NO_WAIT to return immediately.
 * @return Number of ready objects on success, -ETIMEDOUT on timeout, or negative
 *         error code on failure.
 */
int8_t k_poll(struct k_pollfd *fds, uint8_t nfds, k_timeout_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_POLL_H */
