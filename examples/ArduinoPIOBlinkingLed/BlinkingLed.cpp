/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Arduino.h>

#include <avrtos.h>
#include <avrtos/kernel.h>
#include <avrtos/misc/led.h>

void setup(void)
{
	pinMode(LED_BUILTIN, OUTPUT);
}

void loop(void)
{
	digitalWrite(LED_BUILTIN, HIGH);
	k_sleep(K_MSEC(200u));
	digitalWrite(LED_BUILTIN, LOW);
	k_sleep(K_MSEC(200u));
}