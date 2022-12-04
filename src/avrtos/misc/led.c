/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "led.h"

#include <avrtos/drivers/gpio.h>

#define PORT GPIOB_DEVICE

#if defined(__AVR_ATmega328P__)
#	define BUILTIN_LED_PIN (5u)
#elif defined(__AVR_ATmega2560__)
#       define BUILTIN_LED_PIN (7u)	
#else
#	define BUILTIN_LED_PIN 0
#endif

void led_init(void)
{
	gpio_pin_init(PORT, BUILTIN_LED_PIN, GPIO_OUTPUT, 0u);
}

void led_on(void)
{
	gpio_write_pin_state(PORT, BUILTIN_LED_PIN, STATE_HIGH);
}

void led_off(void)
{
	gpio_write_pin_state(PORT, BUILTIN_LED_PIN, STATE_LOW);
}

void led_set(uint8_t state)
{
	gpio_write_pin_state(PORT, BUILTIN_LED_PIN, state);
}

void led_toggle(void)
{
	gpio_toggle_pin(PORT, BUILTIN_LED_PIN);
}
