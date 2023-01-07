/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "prng.h"

K_NOINLINE uint32_t z_shift_lfsr(uint32_t *lfsr, uint32_t poly_mask)
{
        uint32_t feedback = *lfsr & 1;
        *lfsr >>= 1u;
        if (feedback) {
                *lfsr ^= poly_mask;
        }
        return *lfsr;
}

uint16_t k_prng_get(struct k_prng *prng)
{
        z_shift_lfsr(&prng->lfsr32, K_PRNG_POLY_MASK_32);

        const uint16_t lfsr32 = z_shift_lfsr(&prng->lfsr32, K_PRNG_POLY_MASK_32);
        const uint16_t lfsr31 = z_shift_lfsr(&prng->lfsr31, K_PRNG_POLY_MASK_31);

        return lfsr32 ^ lfsr31;
}

uint32_t k_prng_get_u32(struct k_prng* prng)
{
        const uint32_t rdm1 = k_prng_get(prng);
        const uint32_t rdm2 = k_prng_get(prng);

        return rdm1 | (rdm2 << 16);
}

void k_prng_get_buffer(struct k_prng *prng,
        uint8_t *buffer, uint16_t len)
{
        for (uint16_t i = 0; i < (len >> 1); i++) {
                ((uint16_t *)buffer)[i] = k_prng_get(prng);
        }

        if (len & 1)
                buffer[len - 1] = (uint8_t)k_prng_get(prng);
}
