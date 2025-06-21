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

#include "avrtos/defines.h"
#include "avrtos/deprecated.h"
#include "avrtos/kernel.h"
#include "avrtos/tickless.h"
#include "gpio.h"
#include "serial.h"

#define LOG_LEVEL LOG_LEVEL_DBG

static void task(void *arg)
{
    (void)arg;

    for (;;) {
        serial_transmit(k_thread_get_current()->symbol);
        k_sleep(K_MSEC(100u));
    }
}

K_THREAD_DEFINE(t1, task, 0x100, K_COOPERATIVE, NULL, 'a');
K_THREAD_DEFINE(t2, task, 0x100, K_COOPERATIVE, NULL, 'b');
K_THREAD_DEFINE(t3, task, 0x100, K_COOPERATIVE, NULL, 'c');

void print_time(void)
{
    struct z_tickless_timespec tls;
    struct timespec ts;

    z_tickless_time_get(&tls);
    z_tickless_spec_convert(&tls, &ts);

    printf_P(PSTR("Tickless time: "));
    serial_u32(ts.tv_sec);
    printf_P(PSTR("."));
    printf_P(PSTR("%03u\n"), ts.tv_msec);
}

int main(void)
{
    serial_init();
    z_tickless_init();

    printf_P(PSTR("AVRTOS Tickless Example\n"));

    for (;;) {
		gpiol_pin_toggle(GPIOF, 4);
        k_sleep(K_MSEC(63));
        // print_time();

		// uint8_t key = irq_lock();
		// z_tickless_sched_ms(30);
		// irq_unlock(key);
    }

    k_stop();
}