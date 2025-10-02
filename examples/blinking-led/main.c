/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
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