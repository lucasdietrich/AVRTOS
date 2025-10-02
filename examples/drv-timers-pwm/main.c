/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/timer.h>
#include <avrtos/logging.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/io.h>
#define LOG_LEVEL LOG_LEVEL_DBG

ISR(TIMER1_COMPA_vect)
{
    serial_transmit('A');
}

ISR(TIMER1_COMPB_vect)
{
    serial_transmit('B');
}

ISR(TIMER1_COMPC_vect)
{
    serial_transmit('C');
}

ISR(TIMER1_OVF_vect)
{
    serial_transmit('0');
}

ISR(TIMER1_CAPT_vect)
{
    serial_transmit('=');
}

int main(void)
{
    serial_init();

    const struct timer_config config = {
        .mode      = TIMER_MODE_FAST_PWM_10bit,
        .counter   = 0u,
        .prescaler = TIMER_PRESCALER_1024,
        .timsk     = 0u,
    };

    /* Configure timer1 pins as output */
    gpiol_pin_init(GPIOB, 5u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOB, 6u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOB, 7u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);

    ll_timer16_init(TIMER1_DEVICE, timer_get_index(TIMER1_DEVICE), &config);

    struct timer_channel_compare_config comp_conf = {
        .mode  = TIMER_CHANNEL_COMP_MODE_SET,
        .value = 0x1ffu,
    };
    ll_timer16_channel_configure(TIMER1_DEVICE, TIMER_CHANNEL_A, &comp_conf);
    comp_conf.value = 100u;
    ll_timer16_channel_configure(TIMER1_DEVICE, TIMER_CHANNEL_B, &comp_conf);
    comp_conf.mode = TIMER_CHANNEL_COMP_MODE_NORMAL;
    ll_timer16_channel_configure(TIMER1_DEVICE, TIMER_CHANNEL_C, &comp_conf);

    ll_timer_set_enable_int_mask(timer_get_index(TIMER1_DEVICE),
                                 0x2Fu); /* Enable all interupts */

    for (;;) {
        led_toggle();
        k_sleep(K_MSEC(500u));
    }
}