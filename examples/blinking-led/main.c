/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/kernel.h>
#include <avrtos/misc/led.h>

#define PERIOD_MS 200u

int main(void)
{
	led_init();

	for (;;) {
		led_toggle();
		k_sleep(K_MSEC(PERIOD_MS));
	}
}