/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "semaphore.h"

#include <util/atomic.h>

#include "avrtos/poll.h"
#include "avrtos/sys.h"
#include "debug.h"
#include "kernel.h"
#include "kernel_private.h"

#define K_MODULE K_MODULE_SEMAPHORE

int8_t k_sem_init(struct k_sem *sem, uint8_t initial_count, uint8_t limit)
{
    Z_ARGS_CHECK(sem && limit) return -EINVAL;

    sem->limit = limit;
    sem->count = MIN(limit, initial_count);
    dlist_init(&sem->waitqueue);

    return 0;
}

int8_t k_sem_take(struct k_sem *sem, k_timeout_t timeout)
{
    Z_ARGS_CHECK(sem) return -EINVAL;

    int8_t ret        = 0;
    const uint8_t key = irq_lock();

    if (sem->count > 0) {
        /* Semaphore is available, decrement count */
        sem->count--;
    } else {
        /* Semaphore is not available, wait for it */
        ret = z_pend_current_on(&sem->waitqueue, timeout);
    }

    irq_unlock(key);

    if (ret == 0) {
        __Z_DBG_SEM_TAKE(z_ker.current);
    }

    return ret;
}

struct k_thread *k_sem_give(struct k_sem *sem)
{
    Z_ARGS_CHECK(sem) return NULL;

    struct k_thread *thread = NULL;
    const uint8_t key       = irq_lock();

    __Z_DBG_SEM_GIVE(z_ker.current);

    /* Wake up the first thread in the wait queue if any */
    thread = z_unpend_first_thread(&sem->waitqueue);

    if (thread == NULL) {
        /* No threads are waiting, increment the semaphore count */
        if (sem->count < sem->limit) {
            sem->count++;
        }
    }

    irq_unlock(key);

    return thread;
}

#if CONFIG_POLLING
/**
 * @brief Set up polling for semaphore availability.
 *
 * This internal function is called by k_poll() to set up polling on a semaphore.
 * If the semaphore is available (count > 0), it returns 0 indicating the object
 * is ready. Otherwise, it adds the polling descriptor to the semaphore's wait
 * queue and returns -EAGAIN to indicate the thread should wait.
 *
 * @param sem Pointer to the semaphore to poll
 * @param pollfd Pointer to the poll file descriptor
 * @return 0 if semaphore is available, -EAGAIN if thread should wait
 */
int8_t z_sem_setup_pollin(struct k_sem *sem, struct k_pollfd *pollfd)
{
    if (sem->count > 0) {
        /* Semaphore is available, no need to wait */
        return 0;
    }

    /* Add the current thread to the semaphore's wait queue */
    dlist_append(&sem->waitqueue, &pollfd->_wqhandle.tie);
    pollfd->_wqhandle.flags = Z_WQ_FLAG_POLLIN;

    return -EAGAIN; // Indicate that the thread is now waiting
}
#endif /* CONFIG_POLLING */

__kernel int8_t k_sem_cancel_wait(struct k_sem *sem)
{
    __ASSERT_NOTNULL(sem);

    int8_t ret;
    const uint8_t key = irq_lock();

    ret = (int8_t)z_cancel_all_pending(&sem->waitqueue);

    irq_unlock(key);

    return ret;
}
