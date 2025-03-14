/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "exti.h"

int8_t exti_configure(uint8_t exti, uint8_t isc)
{
    Z_ARGS_CHECK(exti < EXTI_COUNT) return -EINVAL;

#if CONFIG_KERNEL_ARGS_CHECKS
    isc &= 0x3u;
#endif

    const uint8_t regn       = exti >> 2u;
    const uint8_t group      = exti & 0x03u;
    const uint8_t group_mask = (BIT(ISCn0) | BIT(ISCn1)) << (group << 1u);
    const uint8_t eicrn      = EXTI_CTRL_DEVICE->EICRn[regn];

    EXTI_CTRL_DEVICE->EICRn[regn] = (eicrn & ~group_mask) | (isc << (group << 1u));

    return 0;
}