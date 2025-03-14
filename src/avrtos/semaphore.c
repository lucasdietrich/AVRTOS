/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "semaphore.h"

#include <util/atomic.h>

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

__kernel int8_t k_sem_cancel_wait(struct k_sem *sem)
{
    __ASSERT_NOTNULL(sem);

    int8_t ret;
    const uint8_t key = irq_lock();

    ret = (int8_t)z_cancel_all_pending(&sem->waitqueue);

    irq_unlock(key);

    return ret;
}
