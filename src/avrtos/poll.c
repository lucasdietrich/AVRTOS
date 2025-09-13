/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "poll.h"
#include "avrtos/kernel.h"

int8_t k_poll(struct k_pollfd *fds, uint16_t nfds, k_timeout_t timeout)
{
    int8_t ret = 0;
    const uint8_t key = irq_lock();

    irq_unlock(key);
    return ret;
}