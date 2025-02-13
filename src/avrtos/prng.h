/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Pseudo-random number generator (PRNG)
 *
 * This module provides an implementation of a PRNG based on the LFSR algorithm as
 * described in the following sources:
 * - https://www.microchip.com/forums/tm.aspx?m=1117824&mpage=1
 * - https://www.maximintegrated.com/en/design/technical-documents/app-notes/4/4400.html
 *
 * The PRNG is not cryptographically secure.
 */

#ifndef _AVRTOS_PRNG_H
#define _AVRTOS_PRNG_H

#include <stdint.h>

#include <avr/io.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure representing the state of the PRNG.
 */
struct k_prng {
	uint32_t lfsr32; /**< 32-bit LFSR state */
	uint32_t lfsr31; /**< 31-bit LFSR state */
};

/* Polynomial masks for the LFSRs */
#define K_PRNG_POLY_MASK_32 0xB4BCD35C /**< Polynomial mask for the 32-bit LFSR */
#define K_PRNG_POLY_MASK_31 0x7A5BC2E3 /**< Polynomial mask for the 31-bit LFSR */

/* Default seed values for the LFSRs */
#define K_PRNG_DEFAULT_LFSR32 0x000ABCDE /**< Default seed value for the 32-bit LFSR */
#define K_PRNG_DEFAULT_LFSR31 0x23456789 /**< Default seed value for the 31-bit LFSR */

/**
 * @brief Macro to initialize a PRNG with specific seed values.
 *
 * @param lfsr32_val Seed value for the 32-bit LFSR.
 * @param lfsr31_val Seed value for the 31-bit LFSR.
 */
#define K_PRNG_INITIALIZER(lfsr32_val, lfsr31_val)                                       \
	{                                                                                    \
		.lfsr32 = lfsr32_val, .lfsr31 = lfsr31_val                                       \
	}

/**
 * @brief Macro to define a PRNG with specific seed values.
 *
 * @param prng_name Name of the PRNG instance.
 * @param lfsr32_val Seed value for the 32-bit LFSR.
 * @param lfsr31_val Seed value for the 31-bit LFSR.
 */
#define K_PRNG_DEFINE(prng_name, lfsr32_val, lfsr31_val)                                 \
	struct k_prng prng_name = K_PRNG_INITIALIZER(lfsr32_val, lfsr31_val)

/**
 * @brief Macro to define a PRNG with default seed values.
 *
 * @param prng_name Name of the PRNG instance.
 */
#define K_PRNG_DEFINE_DEFAULT(prng_name)                                                 \
	K_PRNG_DEFINE(prng_name, K_PRNG_DEFAULT_LFSR32, K_PRNG_DEFAULT_LFSR31)

/**
 * @brief Get a 16-bit pseudo-random number from the PRNG.
 *
 * This function returns a 16-bit pseudo-random number generated by the PRNG.
 *
 * @param prng Pointer to the PRNG structure.
 * @return A 16-bit pseudo-random number.
 */
__kernel uint16_t k_prng_get(struct k_prng *prng);

/**
 * @brief Get a 32-bit pseudo-random number from the PRNG.
 *
 * This function returns a 32-bit pseudo-random number generated by the PRNG.
 *
 * @param prng Pointer to the PRNG structure.
 * @return A 32-bit pseudo-random number.
 */
__kernel uint32_t k_prng_get_u32(struct k_prng *prng);

/**
 * @brief Fill a buffer with pseudo-random data.
 *
 * This function fills the provided buffer with the specified length of pseudo-random
 * data.
 *
 * @param prng Pointer to the PRNG structure.
 * @param buffer Pointer to the buffer where the random data will be stored.
 * @param len Length of the buffer in bytes.
 */
__kernel void k_prng_get_buffer(struct k_prng *prng, uint8_t *buffer, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_PRNG_H */
