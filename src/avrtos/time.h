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

struct timespec {
	uint32_t tv_sec;
	uint16_t tv_msec;
};

/**
 * @brief Get the current time.
 *
 * @param ts
 * @return __kernel
 */
__kernel void k_timespec_get(struct timespec *ts);

/**
 * @brief Set the current time.
 *
 * @param ts
 * @return __kernel
 */
__kernel void k_time_set(uint32_t sec);

__kernel bool k_time_is_set(void);

__kernel uint32_t k_time_get(void);

__kernel void k_time_unset(void);

void k_show_uptime(void);

void k_show_ticks(void);

#ifdef __cplusplus
}
#endif

#endif