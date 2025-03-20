/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_RUST_H
#define _AVRTOS_RUST_H

#include "kernel.h"

uint8_t z_irq_lock(void);

void z_irq_unlock(uint8_t key);

#endif
