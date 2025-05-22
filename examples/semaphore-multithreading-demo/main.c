/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define CRASH_TEST 1

#if CRASH_TEST
#define PERIOD_SEM_GIVE 1000
#define PERIOD_SEM_TAKE PERIOD_SEM_GIVE
#else
#define PERIOD_SEM_GIVE 1000
#define PERIOD_SEM_TAKE 1000
#endif

void waiter_entry(void *);

K_SEM_DEFINE(mysem, 5, 5);
K_THREAD_DEFINE(waiter1, waiter_entry, 0x50, K_PREEMPTIVE, NULL, 'A');
K_THREAD_DEFINE(waiter2, waiter_entry, 0x50, K_PREEMPTIVE, NULL, 'B');
K_THREAD_DEFINE(waiter3, waiter_entry, 0x50, K_PREEMPTIVE, NULL, 'C');
K_THREAD_DEFINE(waiter4, waiter_entry, 0x50, K_PREEMPTIVE, NULL, 'D');
K_THREAD_DEFINE(waiter5, waiter_entry, 0x50, K_PREEMPTIVE, NULL, 'E');

int main(void)
{
    led_init();
    serial_init();

    k_thread_dump_all();

    sei();

    while (1) {
        k_sleep(K_MSEC(PERIOD_SEM_GIVE));

        k_sched_lock();
#if !CONFIG_KERNEL_SCHEDULER_DEBUG
        serial_print_p(PSTR("M: giving a semaphore "));
#endif
        // z_sem_debug(&mysem);
        k_sched_unlock();

        k_sem_give(&mysem);
    }
}

void waiter_entry(void *arg)
{
    ARG_UNUSED(arg);

    while (1) {
        uint8_t dbg_sem = k_sem_take(&mysem, K_FOREVER);

        if (dbg_sem == 0) {
            k_sched_lock();
#if !CONFIG_KERNEL_SCHEDULER_DEBUG
            serial_transmit(k_thread_get_current()->symbol);
            serial_printl_p(PSTR(": got a semaphore !"));
#endif
            k_sched_unlock();

            k_sleep(K_MSEC(PERIOD_SEM_TAKE));
        } else {
            serial_printl_p(PSTR("DIDN'T TOOK A SEMAPHORE, KERNEL PROBLEM"));
        }
    }
}