/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_TIME_H_
#define _AVRTOS_TIME_H_

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct timespec {
    uint32_t tv_sec;
    uint16_t tv_msec;
};

/*___________________________________________________________________________*/

K_NOINLINE void k_timespec_get(struct timespec *ts);

K_NOINLINE void k_time_set(uint32_t sec);

K_NOINLINE bool k_time_is_set(void);

K_NOINLINE uint32_t k_time_get(void);

K_NOINLINE void k_time_unset(void);

void k_show_uptime(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif