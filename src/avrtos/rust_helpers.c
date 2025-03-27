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

#if !CONFIG_THREAD_JOIN
#error "CONFIG_THREAD_JOIN must be enabled for RUST support"
#endif

uint8_t z_irq_lock(void)
{
	return irq_lock();
}

void z_irq_unlock(uint8_t key)
{
	irq_unlock(key);
}

k_timeout_t z_ms_to_ticks(uint32_t ms)
{
	return K_MSEC(ms);
}

#endif