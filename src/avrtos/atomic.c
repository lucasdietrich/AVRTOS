/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atomic.h"

#if CONFIG_KERNEL_ATOMIC_API

void atomic_clear_bit(atomic_t *target, uint8_t bit)
{
        atomic_and(target, ~BIT(bit));
}

void atomic_set_bit(atomic_t *target, uint8_t bit)
{
        atomic_or(target, BIT(bit));
}

void atomic_set_bit_to(atomic_t *target, uint8_t bit, bool val)
{
        if (val) {
                atomic_or(target, BIT(bit));
        } else {
                atomic_and(target, ~BIT(bit));
        }
}

bool atomic_test_bit(atomic_t *target, uint8_t bit)
{
        const atomic_val_t mask =  BIT(bit);
        const atomic_val_t old = atomic_get(target);

        return (old & mask) != 0;
}

bool atomic_test_and_clear_bit(atomic_t *target, uint8_t bit)
{
        const atomic_val_t mask =  BIT(bit);
        const atomic_val_t old = atomic_and(target, ~mask);

        return (old & mask) != 0;
}

bool atomic_test_and_set_bit(atomic_t *target, uint8_t bit)
{
        const atomic_val_t mask =  BIT(bit);
        const atomic_val_t old = atomic_or(target, mask);

        return (old & mask) != 0;
}

bool atomic_cas2(atomic_t *target, atomic_val_t cmd, atomic_val_t val)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (*target == cmd) {
			*target = val;
			return true;
		}
	}
	return false;
}

#endif /* CONFIG_KERNEL_ATOMIC_API */