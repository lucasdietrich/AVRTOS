/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "led.h"

void led_set(uint8_t state)
{
        if (state) {
                led_on();
        } else {
                led_off();
        }
}