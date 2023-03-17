/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos.h>
#include <avrtos/kernel.h>
#include <avrtos/misc/led.h>

void setup(void)
{
	led_init();
}

void loop(void)
{
	led_toggle();
	k_sleep(K_MSEC(200u));
}