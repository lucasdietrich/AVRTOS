/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_ATOMIC_H_
#define _AVRTOS_ATOMIC_H_

#include <stdbool.h>

#include <avrtos/defines.h>

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <util/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief There are very few (or none) atomic instructions in AVR
 * architectures 5 and 6.
 *
 * So to make atomic operations, the only way is to disable
 * interrupts temporarily.
 */

#define K_ATOMIC_INIT(val)	   ((atomic_t)(val))
#define K_ATOMIC_DEFINE(name, val) atomic_t name = K_ATOMIC_INIT(val)

#define ATOMIC_SIZE_BITS (sizeof(atomic_val_t) * 8)

/* for further uint32_t atomic support */
#define ATOMIC_BIT(bit)	       (1 << (bit))
#define ATOMIC_BYTE(addr, bit) ((addr) + ((bit) / ATOMIC_SIZE_BITS))

typedef uint8_t atomic_val_t;
typedef uint8_t atomic_t;

/**
 * @brief Get the current value of an atomic variable.
 *
 * @param target
 * @return K_NOINLINE
 */
K_NOINLINE atomic_val_t atomic_get(atomic_t *target);

/**
 * @brief Clear the value of an atomic variable but do not return the old value.
 *
 * @param target
 * @return K_NOINLINE
 */
K_NOINLINE void atomic_blind_clear(atomic_t *target);

/**
 * @brief Clear the value of an atomic variable.
 *  Return the old value.
 *
 * @param target
 * @return K_NOINLINE
 */
K_NOINLINE atomic_val_t atomic_clear(atomic_t *target);

/**
 * @brief Calculate the OR of value and the atomic variable,
 *  and store the result in the atomic variable.
 *  Return the old value.
 *
 * @param target
 * @param value
 * @return K_NOINLINE
 */
K_NOINLINE atomic_val_t atomic_or(atomic_t *target, atomic_val_t value);

/**
 * @brief Calculate the XOR of value and the atomic variable,
 *  and store the result in the atomic variable.
 *  Return the old value.
 *
 * @param target
 * @param value
 * @return K_NOINLINE
 */
K_NOINLINE atomic_val_t atomic_xor(atomic_t *target, atomic_val_t value);

/**
 * @brief Calculate the AND of value and the atomic variable,
 *  and store the result in the atomic variable.
 *  Return the old value.
 *
 * @param target
 * @param value
 * @return K_NOINLINE
 */
K_NOINLINE atomic_val_t atomic_and(atomic_t *target, atomic_val_t value);

/**
 * @brief Increment the atomic variable by one.
 *  Return the current value.
 *
 * Note: return value is not the old value.
 *
 * @param target
 * @return K_NOINLINE
 */
K_NOINLINE atomic_val_t atomic_inc(atomic_t *target);

/**
 * @brief Decrement the atomic variable by one.
 *  Return the current value.
 *
 * Note: return value is not the old value.
 *
 * @param target
 * @return K_NOINLINE
 */
K_NOINLINE atomic_val_t atomic_dec(atomic_t *target);

/**
 * @brief Clear the bit at position @p bit in the atomic variable.
 *
 * @param target
 * @param bit
 * @return K_NOINLINE
 */
K_NOINLINE void atomic_clear_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Set the bit at position @p bit in the atomic variable.
 *
 * @param target
 * @param bit
 * @return K_NOINLINE
 */
K_NOINLINE void atomic_set_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Set the bit at position @p bit to value @p val in the atomic variable.
 *
 * @param target
 * @param bit
 * @param val
 * @return K_NOINLINE
 */
K_NOINLINE void atomic_set_bit_to(atomic_t *target, uint8_t bit, bool val);

/**
 * @brief Test the bit at position @p bit in the atomic variable.
 *
 * @param target
 * @param bit
 * @return K_NOINLINE
 */
K_NOINLINE bool atomic_test_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Test the bit at position @p bit in the atomic variable and clear it.
 *
 * @param target
 * @param bit
 * @return K_NOINLINE
 */
K_NOINLINE bool atomic_test_and_clear_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Test the bit at position @p bit in the atomic variable and set it.
 *
 * @param target
 * @param bit
 * @return K_NOINLINE
 */
K_NOINLINE bool atomic_test_and_set_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Compare and set, Compare the atomic variable value with @p cmd value
 * 	- If equal update the atomic variable value with @p val value
 * 	- Left the atomic variable value unchanged if not equal
 *
 * Return true if the atomic variable value was updated, false otherwise.
 *
 * @param target
 * @param value
 * @return K_NOINLINE
 */
K_NOINLINE bool atomic_cas(atomic_t *target, atomic_val_t cmd, atomic_val_t val);

/**
 * @brief see atomic_cas
 *
 * @param target
 * @param cmd
 * @param val
 * @return K_NOINLINE
 */
K_NOINLINE bool atomic_cas2(atomic_t *target, atomic_val_t cmd, atomic_val_t val);

#ifdef __cplusplus
}
#endif

#endif