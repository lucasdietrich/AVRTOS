/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Set of helper functions for the rust API
 */

#ifndef _AVRTOS_RUST_H
#define _AVRTOS_RUST_H

#include "kernel.h"

/**
 * Lock the interrupts
 * @see irq_lock
 *
 * @return the key to unlock the interrupts
 */
uint8_t z_irq_lock(void);

/**
 * Unlock the interrupts
 * @see irq_unlock
 *
 * @param key the key to unlock the interrupts
 */
void z_irq_unlock(uint8_t key);

/**
 * Convert milliseconds to ticks
 * @see K_MSEC
 *
 * @param ms the milliseconds to convert
 * @return the converted ticks
 */
k_timeout_t z_ms_to_ticks(uint32_t ms);

#endif
