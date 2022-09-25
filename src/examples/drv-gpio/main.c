/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/kernel.h>
#include <avrtos/drivers/gpio.h>

#define PINn PIN7
#define GPIO GPIOB_DEVICE

int main(void)
{
	irq_enable();
	
	gpio_init(GPIO, 0xFF, 0x00);

	for (;;) {
		GPIO->PIN = BIT(PINn);
		_delay_us(10);
	}
}