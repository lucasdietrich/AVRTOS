/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/drivers/exti.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/misc/led.h>

void __fault_hook(void)
{
    serial_transmit('!');
}

int main(void)
{
    serial_init();
    led_init();
    led_on();

    k_sleep(K_MSEC(1000));

    __fault(K_FAULT_ANY);
}