/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gpio.h"

void gpio_init(GPIO_Device *gpio, uint8_t dir_mask, uint8_t pullup_mask)
{
	gpio->DDR = dir_mask;
	gpio->PORT = pullup_mask;
}

void gpio_pin_init(GPIO_Device *gpio, uint8_t pin, uint8_t dir, uint8_t pullup)
{
	if (dir == GPIO_INPUT) {
		gpio->DDR = gpio->DDR & ~BIT(pin);
	} else {
		gpio->DDR = gpio->DDR | BIT(pin);
	}

	/* represent either pullup (if input) or output level (if output)*/
	if (pullup == GPIO_INPUT_NO_PULLUP) {
		gpio->PORT = gpio->PORT & ~BIT(pin);
	} else {
		gpio->PORT = gpio->PORT | BIT(pin);
	}
}