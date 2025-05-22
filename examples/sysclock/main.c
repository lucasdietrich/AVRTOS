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

#include "avrtos/sys.h"

#define PERIOD 1000

void thread_led(void *p);

K_THREAD_DEFINE(ledon, thread_led, 0x100, K_PRIO_DEFAULT, NULL, 'L');

int main(void)
{
    led_init();
    serial_init();

    k_thread_dump_all();

    k_sleep(K_FOREVER);
}

void thread_led(void *arg)
{
    ARG_UNUSED(arg);

    while (1) {
        k_thread_dump(k_thread_get_current());

        led_on();
        k_sleep(K_MSEC(PERIOD));
        led_off();
        k_sleep(K_MSEC(PERIOD));
    }
}