/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* This example demonstrates how to periodically toggle pins PB5, PB6, PB7 at a
 * certain frequency using the CTC mode of the timer 1 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/timer.h>
#include <avrtos/logging.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/io.h>
#define LOG_LEVEL LOG_LEVEL_DBG

#define TIMER_DEVICE TIMER1_DEVICE

/* 50Hz signals */
/* Period range is [256us to 8.388sec] */
#define PERIOD_US 20000llu

int main(void)
{
    serial_init();

    const struct timer_config config = {
        .mode      = TIMER_MODE_CTC,
        .counter   = 0u,
        .prescaler = TIMER_PRESCALER_1024,
        .timsk     = 0u,
    };

    gpiol_pin_init(GPIOB, 5u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOB, 6u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOB, 7u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);

    ll_timer16_init(TIMER_DEVICE, timer_get_index(TIMER_DEVICE), &config);

    struct timer_channel_compare_config comp_conf = {
        .mode  = TIMER_CHANNEL_COMP_MODE_TOGGLE,
        .value = TIMER_CALC_COUNTER_VALUE(PERIOD_US >> 1u, 1024u),
    };
    ll_timer16_channel_configure(TIMER_DEVICE, TIMER_CHANNEL_A, &comp_conf);
    ll_timer16_channel_configure(TIMER_DEVICE, TIMER_CHANNEL_B, &comp_conf);
    ll_timer16_channel_configure(TIMER_DEVICE, TIMER_CHANNEL_C, &comp_conf);

    k_sleep(K_FOREVER);
}