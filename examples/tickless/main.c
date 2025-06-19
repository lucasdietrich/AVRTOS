/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/drivers/timer.h>
#include <avrtos/logging.h>

#include "avrtos/deprecated.h"
#include "avrtos/kernel.h"
#include "avrtos/tickless.h"
#include "serial.h"

#define LOG_LEVEL LOG_LEVEL_DBG

static void task(void *p)
{
    k_sleep(K_SECONDS(5u));
}

K_THREAD_DEFINE(t1, task, 0x100, K_COOPERATIVE, NULL, 'B');

void print_time(void)
{
    struct z_tickless_timespec tls;
    struct timespec ts;

    z_tickless_time_get(&tls);
    z_tickless_spec_convert(&tls, &ts);

    printf_P(PSTR("software counter: "));
    serial_u32(tls.software_counter);
    printf_P(PSTR(", hardware counter: %u\n"), tls.hardware_counter);

    // printf_P(PSTR("Tickless time: "));
    // serial_u32(ts.tv_sec);
    // printf_P(PSTR("."));
    // printf_P(PSTR("%03u\n"), ts.tv_msec);
}

int main(void)
{
    z_tickless_init();
    serial_init();

    printf_P(PSTR("AVRTOS Tickless Example\n"));

    for (;;) {
        k_sleep(K_MSEC(100));
        // print_time();

		uint8_t key = irq_lock();
		z_tickless_sched_next_ms(30);
		irq_unlock(key);
    }

    k_stop();
}