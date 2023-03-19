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
 * @brief Init builtin LED
 */
void led_init(void);

/**
 * @brief Set builtin LED on
 */
void led_on(void);

/**
 * @brief Set builtin LED off
 */
void led_off(void);

/**
 * @brief Set builtin LED state
 */
void led_set(uint8_t state);

/**
 * @brief Toggle builtin LED
 */
void led_toggle(void);

/**
 * @brief Get builtin LED state
 */
uint8_t led_get(void);

#ifdef __cplusplus
}
#endif

#endif