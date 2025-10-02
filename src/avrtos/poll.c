/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
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
#include <stdio.h>

#include <avr/pgmspace.h>

#include "avrtos/fifo.h"
#include "avrtos/kernel.h"
#include "avrtos/kernel_private.h"
#include "avrtos/msgq.h"
#include "avrtos/mutex.h"
#include "avrtos/poll.h"
#include "avrtos/semaphore.h"

#if CONFIG_POLLING == 1

extern int8_t z_sem_setup_pollin(struct k_sem *sem, struct k_pollfd *pollfd);
extern int8_t z_mutex_setup_pollin(struct k_mutex *mutex, struct k_pollfd *pollfd);
extern int8_t z_fifo_setup_pollin(struct k_fifo *fifo, struct k_pollfd *pollfd);
extern int8_t z_msgq_setup_pollin(struct k_msgq *msgq, struct k_pollfd *pollfd);
extern int8_t z_msgq_setup_pollout(struct k_msgq *msgq, struct k_pollfd *pollfd);

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
        struct k_pollfd *pfd = &pfds[i];

        pfd->revents = 0;
        switch (pfd->type) {
        case K_POLL_TYPE_SEM:
            ret = z_sem_setup_pollin(pfd->obj.sem, pfd);
            break;
        case K_POLL_TYPE_MUTEX:
            ret = z_mutex_setup_pollin(pfd->obj.mutex, pfd);
            break;
        case K_POLL_TYPE_FIFO:
            ret = z_fifo_setup_pollin(pfd->obj.fifo, pfd);
            break;
        case K_POLL_TYPE_MSGQ_GET:
            ret = z_msgq_setup_pollin(pfd->obj.msgq, pfd);
            break;
        case K_POLL_TYPE_MSGQ_PUT:
            ret = z_msgq_setup_pollout(pfd->obj.msgq, pfd);
            break;
        default:
            ret = -EINVAL;
            goto exit;
        }

        if (ret == 0) {
            pfd->revents |= K_POLL_READY;
            nfds = i; // number of processed fds to cleanup
            goto ready;
        }

        pfd->_thread = k_thread_get_current();
    }

    ret = z_pend_current_on(NULL, timeout);

ready:
    for (uint_fast8_t i = 0; i < nfds; i++) {
        struct k_pollfd *pfd = &pfds[i];

        if (pfd->revents & K_POLL_READY) {
            /* If the object is ready, it has already been removed from the wait object
             * queue */
            continue;
        }

        switch (pfd->type) {
        case K_POLL_TYPE_SEM:
        case K_POLL_TYPE_MUTEX:
        case K_POLL_TYPE_FIFO:
        case K_POLL_TYPE_MSGQ_GET:
        case K_POLL_TYPE_MSGQ_PUT:
            dlist_remove(&pfd->_wqhandle.tie);
            break;
        default:
            break;
        }
    }

exit:
    irq_unlock(key);
    return ret;
}

#endif /* CONFIG_POLLING */