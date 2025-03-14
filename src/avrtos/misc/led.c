/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "led.h"

#include <avrtos/drivers/gpio.h>

#if defined(__AVR_ATmega328P__)
#define BUILTIN_LED_PORT GPIOB_DEVICE
#define BUILTIN_LED_PIN  (5u)
#elif ARDUINO_AVR_MEGA2560 || defined(__AVR_ATmega2560__)
#define BUILTIN_LED_PORT GPIOB_DEVICE
#define BUILTIN_LED_PIN  (7u)
#else
#define BUILTIN_LED_PORT GPIOB_DEVICE
#define BUILTIN_LED_PIN  (0u)
#endif

void led_init(void)
{
    gpiol_pin_init(BUILTIN_LED_PORT, BUILTIN_LED_PIN, GPIO_OUTPUT, 0u);
}

void led_on(void)
{
    gpiol_pin_write_state(BUILTIN_LED_PORT, BUILTIN_LED_PIN, STATE_HIGH);
}

void led_off(void)
{
    gpiol_pin_write_state(BUILTIN_LED_PORT, BUILTIN_LED_PIN, STATE_LOW);
}

void led_set(uint8_t state)
{
    gpiol_pin_write_state(BUILTIN_LED_PORT, BUILTIN_LED_PIN, state);
}

void led_toggle(void)
{
    gpiol_pin_toggle(BUILTIN_LED_PORT, BUILTIN_LED_PIN);
}

uint8_t led_get(void)
{
    return gpiol_pin_read_state(BUILTIN_LED_PORT, BUILTIN_LED_PIN);
}