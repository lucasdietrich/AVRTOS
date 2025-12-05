/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief watchdog.c Add support for a shared watchdog between all threads.
 * - Watchdog is reseted only if all critical threads responded in time.
 *
 * - At least one thread should be register in order to use this feature.
 * - Support 8 threads maximum.
 */

#include "watchdog.h"

#if CONFIG_WATCHDOG

#include <avrtos/avrtos.h>

#include <avr/wdt.h>

/**
 * @brief Build the mask :
 * 	ALIVE_THREAD_GET_MASK(1) = 0b0001
 * 	ALIVE_THREAD_GET_MASK(2) = 0b0011
 * 	ALIVE_THREAD_GET_MASK(3) = 0b0111
 */
#define ALIVE_THREAD_GET_MASK(count) ((1U << (count)) - 1U)

static K_ATOMIC_DEFINE(alive_threads, 0x00U);
static K_ATOMIC_DEFINE(threads_count, 0x00U);

/**
 * @brief Register a thread as critical.
 * This thread should call the alive() function frequently in
 * order to prvent the watchdog to reset the MCU.
 *
 * @return uint8_t The thread id, this number is used to call the alive() function.
 */
uint8_t critical_thread_register(void)
{
    return atomic_inc(&threads_count) - 1;
}

/**
 * @brief Tell the watchdog that the given thread is still alive.
 *
 * @param thread_id : index of the thread (returned by @see critical_thread_register )
 */
void alive(uint8_t thread_id)
{
    /* say "I'm alive" */
    atomic_clear_bit(&alive_threads, BIT(thread_id));

    /* reset the watchdog as soon as all threads notified their are alive*/
    if (atomic_cas(&alive_threads, 0x00, ALIVE_THREAD_GET_MASK(threads_count))) {
        wdt_reset();
    }
}
#endif