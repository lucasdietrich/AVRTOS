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

void thread_led_on(void *p);
void thread_led_off(void *p);

K_THREAD_DEFINE(ledon, thread_led_on, 0x120, K_PREEMPTIVE, NULL, 'O');
K_THREAD_DEFINE(ledoff, thread_led_off, 0x140, K_PREEMPTIVE, NULL, 'F');

K_MUTEX_DEFINE(mymutex);

int main(void)
{
    led_init();
    serial_init();

    k_thread_dump_all();

    z_print_runqueue();

    sei();

    for (;;) {
    }
}

void thread_led_on(void *arg)
{
    ARG_UNUSED(arg);

    while (1) {
        k_mutex_lock(&mymutex, K_FOREVER);

        led_on();

        k_sleep(K_MSEC(1000));

        k_mutex_unlock(&mymutex);
    }
}

void thread_led_off(void *arg)
{
    ARG_UNUSED(arg);

    while (1) {
        k_mutex_lock(&mymutex, K_FOREVER);

        led_off();

        k_sleep(K_MSEC(1000));

        k_mutex_unlock(&mymutex);
    }
}