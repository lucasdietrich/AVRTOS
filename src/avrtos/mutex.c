/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mutex.h"

#include <util/atomic.h>

#include "avrtos/poll.h"
#include "debug.h"
#include "kernel.h"
#include "kernel_private.h"

#define K_MODULE K_MODULE_MUTEX

#define Z_MUTEX_UNLOCKED_VALUE 0u

int8_t k_mutex_init(struct k_mutex *mutex)
{
    Z_ARGS_CHECK(mutex) return -EINVAL;

    mutex->lock  = Z_MUTEX_UNLOCKED_VALUE;
    mutex->owner = NULL;
    dlist_init(&mutex->waitqueue);

    return 0;
}

int8_t k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout)
{
    Z_ARGS_CHECK(mutex) return -EINVAL;

    int8_t ret        = 0;
    const uint8_t key = irq_lock();

    if (mutex->lock == Z_MUTEX_UNLOCKED_VALUE) {
        /* Mutex is available, acquire it */
        mutex->lock = 1u;
    } else if (mutex->owner == z_ker.current) {
#if CONFIG_KERNEL_REENTRANCY
        /* Mutex is already owned by the current thread, increment lock count */
        mutex->lock++;
#endif /* CONFIG_KERNEL_REENTRANCY */
        goto exit;
    } else {
        /* Mutex is locked by another thread, wait for it to become available */
        ret = z_pend_current_on(&mutex->waitqueue, timeout);
    }

    if (ret == 0) {
        /* Successfully acquired the mutex */
        __Z_DBG_MUTEX_LOCKED(z_ker.current);
        mutex->owner = z_ker.current;
    }

exit:
    irq_unlock(key);
    return ret;
}

struct k_thread *k_mutex_unlock(struct k_mutex *mutex)
{
    Z_ARGS_CHECK(mutex) return NULL;

    struct k_thread *thread = NULL;
    const uint8_t key       = irq_lock();

    if (mutex->owner != z_ker.current) {
        /* Current thread does not own the mutex, cannot unlock */
        goto exit;
#if CONFIG_KERNEL_REENTRANCY
    } else if (mutex->lock > 1u) {
        /* Reentrant locking: decrement lock count instead of unlocking */
        mutex->lock--;
        goto exit;
#endif /* CONFIG_KERNEL_REENTRANCY */
    }

    __Z_DBG_MUTEX_UNLOCKED(z_ker.current);

    /* There is a new owner, we don't need to unlock the mutex as
     * the mutex owner is changed when returning from the
     * k_mutex_lock() function. Function to where the woken up thread
     * will be returned.
     */
    thread = z_unpend_first_thread(&mutex->waitqueue);
    if (thread == NULL) {
        /* No threads are waiting, fully unlock the mutex */
        mutex->lock  = Z_MUTEX_UNLOCKED_VALUE;
        mutex->owner = NULL;
    }

exit:
    irq_unlock(key);
    return thread;
}

#if CONFIG_POLLING
/**
 * @brief Set up polling for mutex availability.
 *
 * This internal function is called by k_poll() to set up polling on a mutex.
 * If the mutex is available (unlocked), it returns 0 indicating the object
 * is ready. Otherwise, it adds the polling descriptor to the mutex's wait
 * queue and returns -EAGAIN to indicate the thread should wait.
 *
 * @param mutex Pointer to the mutex to poll
 * @param pollfd Pointer to the poll file descriptor
 * @return 0 if mutex is available, -EAGAIN if thread should wait
 */
int8_t z_mutex_setup_pollin(struct k_mutex *mutex, struct k_pollfd *pollfd)
{
    if (mutex->lock == Z_MUTEX_UNLOCKED_VALUE) {
        /* Mutex is available, no need to wait */
        return 0;
    }

    /* Add the current thread to the mutex's wait queue */
    dlist_append(&mutex->waitqueue, &pollfd->_wqhandle.tie);
    pollfd->_wqhandle.flags = Z_WQ_FLAG_POLLIN;

    return -EAGAIN; // Indicate that the thread is now waiting
}
#endif /* CONFIG_POLLING */

int8_t k_mutex_cancel_wait(struct k_mutex *mutex)
{
    __ASSERT_NOTNULL(mutex);

    int8_t ret;
    const uint8_t key = irq_lock();

    ret = (int8_t)z_cancel_all_pending(&mutex->waitqueue);

    irq_unlock(key);

    return ret;
}
