/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/timer.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#define TIMER_DEVICE TIMER3_DEVICE
#define TIMER_vect   TIMER3_COMPA_vect

ISR(TIMER_vect)
{
    serial_transmit('!');
}

int main(void)
{
    serial_init();

    const struct timer_config cfg16 = {
        .mode      = TIMER_MODE_CTC,
        .prescaler = TIMER_PRESCALER_1024,
        .counter   = 10000u,
        .timsk     = BIT(
                OCIEnA), // BIT(OCIEnA) | BIT(OCIEnB) | BIT(OCIEnC) | BIT(TOIEn) | BIT(ICIEn)
    };

    ll_timer16_init(TIMER_DEVICE, timer_get_index(TIMER_DEVICE), &cfg16);

    for (;;) {
        k_msleep(1000u);
    }
}