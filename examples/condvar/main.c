/*
 * Copyright (c) 2026 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Condition Variable Demo
 * =======================
 * Classic producer / consumer scenario:
 *
 *   - One producer thread generates a new item every second, places it in a
 *     shared slot, and calls k_condvar_broadcast() to wake all waiting
 *     consumers.
 *
 *   - Two consumer threads wait on the condvar.  Because k_condvar_broadcast
 *     wakes *both* of them, the standard "re-check the predicate inside a
 *     while loop" idiom is used so the consumer that finds the slot already
 *     taken simply goes back to sleep.
 *
 * Execution flow
 * --------------
 *  1. Both consumers lock the mutex, find nothing ready, call
 *     k_condvar_wait() – this atomically releases the mutex and pends the
 *     thread on the condvar wait-queue.
 *  2. The producer wakes up, locks the mutex, deposits the item, calls
 *     k_condvar_broadcast(), and unlocks the mutex.
 *  3. Both consumers are unblocked; whichever acquires the mutex first sees
 *     data_ready == 1, consumes the item, clears the flag, and releases the
 *     mutex.  The other consumer then acquires the mutex, sees
 *     data_ready == 0, and loops back to wait.
 */

#include <avrtos/avrtos.h>
#include <avrtos/condvar.h>
#include <avrtos/debug.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "avrtos/defines.h"
#include "avrtos/kernel.h"

#include <inttypes.h>
#include <stdio.h>

/* ── Shared state ─────────────────────────────────────────────────────────── */

static volatile uint8_t data_ready = 0;
static volatile uint8_t data_value = 0;

K_MUTEX_DEFINE(data_mutex);
K_CONDVAR_DEFINE(data_cond);

/* ── Thread declarations ──────────────────────────────────────────────────── */

void producer_thread(void *arg);
void consumer_thread(void *arg);

K_THREAD_DEFINE(producer,  producer_thread, 0x110, K_COOPERATIVE, NULL, 'P');
K_THREAD_DEFINE(consumer1, consumer_thread, 0x110, K_COOPERATIVE, NULL, 'A');
K_THREAD_DEFINE(consumer2, consumer_thread, 0x110, K_COOPERATIVE, NULL, 'B');

/* ── main ─────────────────────────────────────────────────────────────────── */

int main(void)
{
    k_sleep(K_FOREVER);
}

/* ── Producer ─────────────────────────────────────────────────────────────── */

void producer_thread(void *arg)
{
    ARG_UNUSED(arg);

    uint8_t count = 0;

    while (1) {
        /* Produce a new item once per second */
        k_sleep(K_MSEC(1000));

        k_mutex_lock(&data_mutex, K_FOREVER);

        data_value = ++count;
        data_ready = 1;

        printf_P(PSTR("P: produced %" PRIu8 "\n"), data_value);

        /* Wake all consumers; they will re-check the predicate themselves */
        k_condvar_broadcast(&data_cond);

        k_mutex_unlock(&data_mutex);
    }
}

/* ── Consumer ─────────────────────────────────────────────────────────────── */

void consumer_thread(void *arg)
{
    ARG_UNUSED(arg);

    const char symbol = (char)k_thread_get_current()->symbol;

    k_mutex_lock(&data_mutex, K_FOREVER);

    while (1) {
        /*
         * Always re-check the predicate after waking – a competing consumer
         * may have already consumed the item (spurious or broadcast wakeup).
         */
        while (!data_ready) {
            k_condvar_wait(&data_cond, &data_mutex, K_FOREVER);
        }

        uint8_t val = data_value;
        data_ready  = 0;

        k_mutex_unlock(&data_mutex);

        printf_P(PSTR("%c: consumed %" PRIu8 "\n"), symbol, val);

        /* Yield so the other consumer gets a chance to run before we race
         * to re-acquire the mutex, otherwise the same thread wins every time. */
        k_yield();

        k_mutex_lock(&data_mutex, K_FOREVER);
    }
}