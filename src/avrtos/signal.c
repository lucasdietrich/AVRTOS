/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "signal.h"

#include <util/atomic.h>

#include "kernel.h"
#include "kernel_private.h"

#define K_MODULE K_MODULE_SIGNAL

int8_t k_signal_init(struct k_signal *sig)
{
    Z_ARGS_CHECK(sig) return -EINVAL;

    sig->signal = 0u;
    sig->flags  = K_POLL_STATE_NOT_READY;
    dlist_init(&sig->waitqueue);

    return 0;
}

int8_t k_signal_raise(struct k_signal *sig, uint8_t value)
{
    Z_ARGS_CHECK(sig) return -EINVAL;

    int8_t ret        = 0;
    const uint8_t key = irq_lock();

    sig->signal = value;
    sig->flags |= K_POLL_STATE_SIGNALED;

    /* Wake up all threads waiting on the signal */
    while (z_unpend_first_thread(&sig->waitqueue) != NULL) {
        ret++;
    }

    irq_unlock(key);

    return ret;
}

int8_t k_poll_signal(struct k_signal *sig, k_timeout_t timeout)
{
    Z_ARGS_CHECK(sig) return -EINVAL;

    int8_t ret;
    const uint8_t key = irq_lock();

    if (TEST_BIT(sig->flags, K_POLL_STATE_SIGNALED)) {
        ret = 0;
    } else {
        ret = z_pend_current_on(&sig->waitqueue, timeout);
    }

    irq_unlock(key);

    return ret;
}

int8_t k_poll_cancel_wait(struct k_signal *sig)
{
    Z_ARGS_CHECK(sig) return -EINVAL;

    int8_t ret;
    const uint8_t key = irq_lock();

    ret = (int8_t)z_cancel_all_pending(&sig->waitqueue);

    irq_unlock(key);

    return ret;
}