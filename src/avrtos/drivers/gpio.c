/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gpio.h"

void gpio_init(GPIO_Device *gpio, uint8_t dir_mask, uint8_t pullup_mask)
{
    gpiol_init(gpio, dir_mask, pullup_mask);
}

void gpio_pin_init(GPIO_Device *gpio, uint8_t pin, uint8_t dir, uint8_t pullup)
{
    gpiol_pin_init(gpio, pin, dir, pullup);
}

void gpio_pin_set_direction(GPIO_Device *gpio, uint8_t pin, uint8_t direction)
{
    gpiol_pin_set_direction(gpio, pin, direction);
}

uint8_t gpio_pin_get_direction(GPIO_Device *gpio, uint8_t pin)
{
    return gpiol_pin_get_direction(gpio, pin);
}

void gpio_pin_set_pullup(GPIO_Device *gpio, uint8_t pin, uint8_t pullup)
{
    gpiol_pin_set_pullup(gpio, pin, pullup);
}

void gpio_pin_write_state(GPIO_Device *gpio, uint8_t pin, uint8_t state)
{
    gpiol_pin_write_state(gpio, pin, state);
}

void gpio_pin_toggle(GPIO_Device *gpio, uint8_t pin)
{
    gpiol_pin_toggle(gpio, pin);
}

uint8_t gpio_pin_read_state(GPIO_Device *gpio, uint8_t pin)
{
    return gpiol_pin_read_state(gpio, pin);
}