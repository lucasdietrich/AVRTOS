/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gpio.h"

void gpio_init(GPIO_Device *gpio, uint8_t dir, uint8_t pullup)
{
	if (dir == GPIO_INPUT) {
		gpio->DDR = 0x00u;
	} else {
		gpio->DDR = 0xFFu;
	}

	if (pullup == GPIO_INPUT_NO_PULLUP) {
		gpio->PORT = 0x00u;
	} else {
		gpio->PORT = 0xFFu;
	}

	gpio->PIN = 0x00u;
}

void gpio_pin_init(GPIO_Device *gpio, uint8_t pin, uint8_t dir, uint8_t pullup)
{
	if (dir == GPIO_INPUT) {
		gpio->DDR = gpio->DDR & ~(1u << pin);
	} else {
		gpio->DDR = gpio->DDR | (1u << pin);
	}

	if (pullup == GPIO_INPUT_NO_PULLUP) {
		gpio->PORT = gpio->PORT & ~(1u << pin);
	} else {
		gpio->PORT = gpio->PORT | (1u << pin);
	}

	gpio->PIN = (gpio->PIN & ~(1u << pin)) | ((gpio->PIN >> pin) & (1u << pin));
}