/*
 * Copyright (c) 2025 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_TICKLESS_H_
#define _AVRTOS_TICKLESS_H_

// FIXME this variable might not be required, but could be deduced from other variables
#define Z_TICKLESS_FAR_SCHEDULING (1u << 0u)

#include "avrtos/defines.h"
#include "kernel.h"
#include "systime.h"

#ifdef __cplusplus
extern "C" {
#endif

struct z_tickless_timespec {
    uint32_t global_counter; // Software counter, incremented on each timer overflow
    uint16_t hardware_counter; // Hardware internal counter, incremented on each timer tick
};

void z_tickless_init(void);

// /**
//  * @brief Schedule the next tickless event with a 1 millisecond resolution.
//  *
//  * @param hundred_us The time in 1 milliseconds to wait until the next tickless event
//  */
// void z_tickless_sched_ms(uint32_t ms);

/**
 * @brief Schedule the next tickless event with a 1 tick resolution.
 *
 * @param ticks The time in ticks to wait until the next tickless event
 */
void z_tickless_sched_ticks(uint32_t ticks);

uint32_t z_tickless_early_sp(bool advance_sp);

// void z_tickless_continue_ms(uint32_t ms);

/**
 * @brief Measure the elapsed ticks since the last tickless event.
 *
 * This function measures the elapsed ticks since the last tickless event
 * and updates the ticks counter accordingly.
 *
 * FIXME doc and args
 * @param ticks Pointer to a variable where the elapsed ticks will be stored.
 * @return The number of elapsed ticks since the last tickless event.
 */
uint32_t z_tickless_elapsed_since_last_sp(void);

void z_tickless_continue_ticks(uint32_t ticks);

void z_tickless_sched_cancel(void);

void z_tickless_time_get(struct z_tickless_timespec *tls);

void z_tickless_spec_convert(struct z_tickless_timespec *tls, struct timespec *ts);

void z_tickless_configure_timeslice(void);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_TICKLESS_H_ */
