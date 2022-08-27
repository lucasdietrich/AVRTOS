/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gpio.h"

void gpio_init(GPIO_Device *gpio, uint8_t dir, uint8_t pullup)
{
	gpio->DDR = (dir == GPIO_INPUT) ? 0x00u : 0xFFu;
	gpio->PORT = (pullup == GPIO_INPUT_NO_PULLUP) ? 0x00u : 0xFFu;
	gpio->PIN = 0x00u;
}

void gpio_pin_init(GPIO_Device *gpio, uint8_t pin, uint8_t dir, uint8_t pullup)
{
	gpio->DDR = (dir == GPIO_INPUT) ? (gpio->DDR & ~(1u << pin)) : (gpio->DDR | (1u << pin));
	gpio->PORT = (pullup == GPIO_INPUT_NO_PULLUP) ? (gpio->PORT & ~(1u << pin)) : (gpio->PORT | (1u << pin));
	gpio->PIN = (gpio->PIN & ~(1u << pin)) | ((gpio->PIN >> pin) & (1u << pin));
}