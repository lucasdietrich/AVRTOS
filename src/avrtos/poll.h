/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_POLL_H
#define _AVRTOS_POLL_H

#include <stdint.h>

#include "avrtos/defines.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct k_pollfd {
    union {
        struct k_sem *sem;      /**< Pointer to a semaphore for polling */
    } obj;
};

int8_t k_poll(struct k_pollfd *fds, uint16_t nfds, k_timeout_t timeout);


#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_POLL_H */
