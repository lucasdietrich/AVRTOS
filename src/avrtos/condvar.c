/*
 * Copyright (c) 2026 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "condvar.h"

#include "avrtos/defines.h"
#include "avrtos/errno.h"
#include "avrtos/kernel.h"
#include "avrtos/kernel_private.h"
#include "avrtos/mutex.h"
#include "avrtos/dstruct/dlist.h"

__kernel int k_condvar_broadcast(struct k_condvar *condvar)
{
    if (!z_user(condvar))
        return -EINVAL;

    int ret           = 0;
    const uint8_t key = irq_lock();

    while (z_unpend_first_thread(&condvar->waitqueue) != NULL) {
        ret++;
    }

    irq_unlock(key);

    return ret;
}

__kernel int k_condvar_init(struct k_condvar *condvar)
{
    if (!z_user(condvar))
        return -EINVAL;

    dlist_init(&condvar->waitqueue);

    return 0;
}

__kernel int
k_condvar_wait(struct k_condvar *condvar, struct k_mutex *mutex, k_timeout_t timeout)
{
    if (!z_user(condvar && mutex))
        return -EINVAL;

    const uint8_t key = irq_lock();

    k_mutex_unlock(mutex);

    int ret = z_pend_current_on(&condvar->waitqueue, timeout);

    irq_unlock(key);

    if (ret == 0) {
        ret = k_mutex_lock(mutex, K_FOREVER);
    }

    return ret;
}