/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _LED_H
#define _LED_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init LED
 */
void led_init(void);

/**
 * @brief Set LED
 */
void led_on(void);

/**
 * @brief Set LED
 */
void led_off(void);

/**
 * @brief Set LED 
 */
void led_set(uint8_t state);

/**
 * @brief Toggle LED
 */
void led_toggle(void);

#ifdef __cplusplus
}
#endif

#endif