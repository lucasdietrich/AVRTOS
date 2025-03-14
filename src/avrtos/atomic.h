/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * AVR architectures have very limited support for atomic instructions.
 * As a result, atomic operations must be implemented by temporarily disabling
 * interrupts to ensure that operations are not interrupted.
 */

#ifndef _AVRTOS_ATOMIC_H_
#define _AVRTOS_ATOMIC_H_

#include <stdbool.h>

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define K_ATOMIC_INIT(val)         ((atomic_t)(val))
#define K_ATOMIC_DEFINE(name, val) atomic_t name = K_ATOMIC_INIT(val)

#define ATOMIC_SIZE_BITS (sizeof(atomic_val_t) * 8)

/* Macros for bit and byte calculations */
#define ATOMIC_BIT(bit)        (1 << (bit))
#define ATOMIC_BYTE(addr, bit) ((addr) + ((bit) / ATOMIC_SIZE_BITS))

typedef uint8_t atomic_val_t;
typedef uint8_t atomic_t;

/**
 * @brief Get the current value of an atomic variable.
 *
 * Note: This function is equivalent to directly reading the 1-byte address.
 *
 * @param target Pointer to the atomic variable.
 * @return The current value of the atomic variable.
 */
__kernel atomic_val_t atomic_get(atomic_t *target);

/**
 * @brief Set the value of an atomic variable.
 *
 * Note: This function is equivalent to directly writing to the 1-byte address.
 *
 * @param target Pointer to the atomic variable.
 * @param value The value to set.
 * @return The old value of the atomic variable.
 */
__kernel atomic_val_t atomic_set(atomic_t *target, uint32_t value);

/**
 * @brief Clear the value of an atomic variable without returning the old value.
 *
 * @param target Pointer to the atomic variable.
 */
__kernel void atomic_blind_clear(atomic_t *target);

/**
 * @brief Clear the value of an atomic variable and return the old value.
 *
 * @param target Pointer to the atomic variable.
 * @return The old value of the atomic variable.
 */
__kernel atomic_val_t atomic_clear(atomic_t *target);

/**
 * @brief Perform a bitwise OR on the atomic variable with the given value.
 *        Store the result in the atomic variable.
 *
 * @param target Pointer to the atomic variable.
 * @param value Value to OR with the atomic variable.
 * @return The old value of the atomic variable.
 */
__kernel atomic_val_t atomic_or(atomic_t *target, atomic_val_t value);

/**
 * @brief Perform a bitwise XOR on the atomic variable with the given value.
 *        Store the result in the atomic variable.
 *
 * @param target Pointer to the atomic variable.
 * @param value Value to XOR with the atomic variable.
 * @return The old value of the atomic variable.
 */
__kernel atomic_val_t atomic_xor(atomic_t *target, atomic_val_t value);

/**
 * @brief Perform a bitwise AND on the atomic variable with the given value.
 *        Store the result in the atomic variable.
 *
 * @param target Pointer to the atomic variable.
 * @param value Value to AND with the atomic variable.
 * @return The old value of the atomic variable.
 */
__kernel atomic_val_t atomic_and(atomic_t *target, atomic_val_t value);

/**
 * @brief Increment the atomic variable by one and return the new value.
 *
 * Note: The return value is the new value, not the old value.
 *
 * @param target Pointer to the atomic variable.
 * @return The new value of the atomic variable.
 */
__kernel atomic_val_t atomic_inc(atomic_t *target);

/**
 * @brief Decrement the atomic variable by one and return the new value.
 *
 * Note: The return value is the new value, not the old value.
 *
 * @param target Pointer to the atomic variable.
 * @return The new value of the atomic variable.
 */
__kernel atomic_val_t atomic_dec(atomic_t *target);

/**
 * @brief Clear the bit at the specified position in the atomic variable.
 *
 * @param target Pointer to the atomic variable.
 * @param bit The bit position to clear.
 */
__kernel void atomic_clear_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Set the bit at the specified position in the atomic variable.
 *
 * @param target Pointer to the atomic variable.
 * @param bit The bit position to set.
 */
__kernel void atomic_set_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Set the bit at the specified position to the given value in the atomic variable.
 *
 * @param target Pointer to the atomic variable.
 * @param bit The bit position to modify.
 * @param val The value to set (true to set, false to clear).
 */
__kernel void atomic_set_bit_to(atomic_t *target, uint8_t bit, bool val);

/**
 * @brief Test whether the bit at the specified position in the atomic variable is set.
 *
 * @param target Pointer to the atomic variable.
 * @param bit The bit position to test.
 * @return True if the bit is set, false otherwise.
 */
__kernel bool atomic_test_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Test whether the bit at the specified position in the atomic variable is set
 *        and clear it.
 *
 * @param target Pointer to the atomic variable.
 * @param bit The bit position to test and clear.
 * @return True if the bit was set before clearing, false otherwise.
 */
__kernel bool atomic_test_and_clear_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Test whether the bit at the specified position in the atomic variable is set
 *        and set it.
 *
 * @param target Pointer to the atomic variable.
 * @param bit The bit position to test and set.
 * @return True if the bit was set before setting, false otherwise.
 */
__kernel bool atomic_test_and_set_bit(atomic_t *target, uint8_t bit);

/**
 * @brief Compare and set operation. Compares the atomic variable value with the provided
 *        command value. If equal, updates the atomic variable with the provided value.
 *        If not equal, leaves the atomic variable unchanged.
 *
 * @param target Pointer to the atomic variable.
 * @param cmd The value to compare against.
 * @param val The value to set if the comparison is successful.
 * @return True if the atomic variable was updated, false otherwise.
 */
__kernel bool atomic_cas(atomic_t *target, atomic_val_t cmd, atomic_val_t val);

/**
 * @brief Alternate implementation of compare and set.
 *
 * @see atomic_cas
 *
 * @param target Pointer to the atomic variable.
 * @param cmd The value to compare against.
 * @param val The value to set if the comparison is successful.
 * @return True if the atomic variable was updated, false otherwise.
 */
__kernel bool atomic_cas2(atomic_t *target, atomic_val_t cmd, atomic_val_t val);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_ATOMIC_H_ */
