/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file poll.c
 * @brief Implementation of the polling API for kernel objects.
 *
 * This file implements the k_poll() function which allows threads to wait
 * for events on multiple kernel objects simultaneously.
 */

#include <stdint.h>

#include <avr/pgmspace.h>

#include "avrtos/errno.h"
#include "avrtos/fifo.h"
#include "avrtos/kernel.h"
#include "avrtos/kernel_private.h"
#include "avrtos/msgq.h"
#include "avrtos/mutex.h"
#include "avrtos/poll.h"
#include "avrtos/semaphore.h"

#if CONFIG_POLLING == 1

/**
 * @brief Poll multiple kernel objects for events.
 *
 * This function implements the core polling logic. It iterates through the
 * provided poll file descriptors, checks if each object is ready, and if not,
 * sets up polling for each object. If any object is immediately ready, it
 * returns immediately. Otherwise, it waits for events or timeout.
 *
 * The function uses a two-phase approach:
 * 1. Setup phase: Check each object and set up polling if needed
 * 2. Wait/Cleanup phase: Wait for events or clean up polling structures
 *
 * @param pfds Array of poll file descriptors
 * @param nfds Number of file descriptors in the array
 * @param timeout Timeout for waiting
 * @return Number of ready objects, or negative error code
 */
int8_t k_poll(struct k_pollfd *pfds, uint8_t nfds, k_timeout_t timeout)
{
    int8_t ret        = 0;
    const uint8_t key = irq_lock();

    for (uint_fast8_t i = 0; i < nfds; i++) {
        struct k_pollfd *pfd    = &pfds[i];
        struct dnode *waitqueue = NULL;

        switch (pfd->type) {
        case K_POLL_TYPE_SEM:
            if (pfd->obj.sem->count == 0) {
                waitqueue            = &pfd->obj.sem->waitqueue;
                pfd->_wqhandle.flags = Z_WQ_FLAG_POLLIN;
            }
            break;
        case K_POLL_TYPE_MUTEX:
            if (pfd->obj.mutex->lock != Z_MUTEX_UNLOCKED_VALUE) {
                waitqueue            = &pfd->obj.mutex->waitqueue;
                pfd->_wqhandle.flags = Z_WQ_FLAG_POLLIN;
            }
            break;
        case K_POLL_TYPE_FIFO:
            if (slist_peek_head(&pfd->obj.fifo->queue) == NULL) {
                waitqueue            = &pfd->obj.fifo->waitqueue;
                pfd->_wqhandle.flags = Z_WQ_FLAG_POLLIN;
            }
            break;
        case K_POLL_TYPE_MSGQ_GET:
            if (pfd->obj.msgq->used_msgs == 0) {
                waitqueue            = &pfd->obj.msgq->waitqueue;
                pfd->_wqhandle.flags = Z_WQ_FLAG_POLLIN;
            }
            break;
        case K_POLL_TYPE_MSGQ_PUT:
            if (pfd->obj.msgq->used_msgs == pfd->obj.msgq->max_msgs) {
                waitqueue            = &pfd->obj.msgq->waitqueue;
                pfd->_wqhandle.flags = Z_WQ_FLAG_POLLOUT;
            }
            break;
        case K_POLL_TYPE_MEM_SLAB:
            if (slab_available(&pfd->obj.mem_slab->allocator) != 0) {
                waitqueue            = &pfd->obj.mem_slab->waitqueue;
                pfd->_wqhandle.flags = Z_WQ_FLAG_POLLIN;
            }
            break;
        default:
            ret = -EINVAL;
            pfd->revents |= K_POLL_ERR;
            continue;
        }

        if (waitqueue != NULL) {
            pfd->revents = 0;
            pfd->_thread = k_thread_get_current();
            dlist_append(waitqueue, &pfd->_wqhandle.tie);
        } else {
            pfd->revents |= K_POLL_READY;
            ret++;
        }
    }

    if (ret == 0) {
        ret = z_pend_current_on(NULL, timeout);
    }

    // Cleanup
    for (uint_fast8_t i = 0; i < nfds; i++) {
        struct k_pollfd *pfd = &pfds[i];

        if (pfd->revents) {
            /* If the object is ready or on error, it has already been removed from the
             * wait object queue */
            continue;
        }

        switch (pfd->type) {
        case K_POLL_TYPE_SEM:
        case K_POLL_TYPE_MUTEX:
        case K_POLL_TYPE_FIFO:
        case K_POLL_TYPE_MSGQ_GET:
        case K_POLL_TYPE_MSGQ_PUT:
        case K_POLL_TYPE_MEM_SLAB:
            dlist_remove(&pfd->_wqhandle.tie);
            break;
        default:
            break;
        }
    }

    irq_unlock(key);
    return ret;
}

#endif /* CONFIG_POLLING */