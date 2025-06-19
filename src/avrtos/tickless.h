/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_TICKLESS_H_
#define _AVRTOS_TICKLESS_H_

#include "avrtos/defines.h"
#include "kernel.h"
#include "systime.h"

#ifdef __cplusplus
extern "C" {
#endif

struct z_tickless_timespec {
    // uint32_t period_us; // FIXME CONSTANT
    uint32_t software_counter; // Software counter, incremented on each timer overflow

    // uint8_t tick_us; // FIXME CONSTANT
    uint16_t hardware_counter; // Hardware internal counter, incremented on each timer tick
};

void z_tickless_init(void);

/**
 * @brief Schedule the next tickless event with a 1 millisecond resolution.
 *
 * @param hundred_us The time in 1 milliseconds to wait until the next tickless event
 */
int8_t z_tickless_sched_next_ms(uint32_t ms);

void z_tickless_time_get(struct z_tickless_timespec *tls);

void z_tickless_spec_convert(struct z_tickless_timespec *tls, struct timespec *ts);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_TICKLESS_H_ */
