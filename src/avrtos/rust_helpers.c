/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rust_helpers.h"

#if CONFIG_RUST

#if CONFIG_KERNEL_THREAD_TERMINATION_TYPE <= 0
#error "CONFIG_KERNEL_THREAD_TERMINATION_TYPE must be configured (> 0) for RUST support"
#endif

uint8_t z_irq_lock(void)
{
	return irq_lock();
}

void z_irq_unlock(uint8_t key)
{
	irq_unlock(key);
}

#endif