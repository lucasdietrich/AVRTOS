/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEVICE_TCN75_H
#define _AVRTOS_DEVICE_TCN75_H

#include <stddef.h>
#include <stdint.h>

#include <avrtos/drivers/i2c.h>
#include <avrtos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

// TCN75 freq 0 (min) > 100 (typ) > 400 (max) kHz

#define TCN75_ADDR_BASE 0x49u

#define TCN75_TEMPERATURE_REGISTER 0U
#define TCN75_CONFIG_REGISTER	   1U
#define TCN75_HISTERESYS_REGISTER  2U
#define TCN75_SETPONT_REGISTER	   3U

#define TCN75_SHUTDOWN_BIT     0U
#define TCN75_NORMAL_OPERATION (0U << TCN75_SHUTDOWN_BIT)
#define TCN75_SHUTDOWN_MODE    (1U << TCN75_SHUTDOWN_BIT)

#define TCN75_COMPINT_BIT     1U
#define TCN75_COMPARATOR_MODE (0U << TCN75_COMPINT_BIT)
#define TCN75_INTERRUPT_MODE  (1U << TCN75_COMPINT_BIT)

#define TCN75_COMPINT_POLARITY_BIT	   2U
#define TCN75_COMPINT_POLARITY_ACTIVE_LOW  (0U << TCN75_COMPINT_POLARITY_BIT)
#define TCN75_COMPINT_POLARITY_ACTIVE_HIGH (1U << TCN75_COMPINT_POLARITY_BIT)

#define TCN75_FAULT_QUEUE_BIT		  3U
#define TCN75_FAULT_QUEUE_NB_CONVERSION_1 (0b00 << TCN75_FAULT_QUEUE_BIT)
#define TCN75_FAULT_QUEUE_NB_CONVERSION_2 (0b01 << TCN75_FAULT_QUEUE_BIT)
#define TCN75_FAULT_QUEUE_NB_CONVERSION_4 (0b10 << TCN75_FAULT_QUEUE_BIT)
#define TCN75_FAULT_QUEUE_NB_CONVERSION_6 (0b11 << TCN75_FAULT_QUEUE_BIT)

#define TCN75_RESOLUTION_BIT   5U
#define TCN75_RESOLUTION_9BIT  (0b00 << TCN75_RESOLUTION_BIT)
#define TCN75_RESOLUTION_10BIT (0b01 << TCN75_RESOLUTION_BIT)
#define TCN75_RESOLUTION_11BIT (0b10 << TCN75_RESOLUTION_BIT)
#define TCN75_RESOLUTION_12BIT (0b11 << TCN75_RESOLUTION_BIT)

#define TCN75_ONESHOT_BIT 7U
#define TCN75_CONTINUOUS  (0U << TCN75_ONESHOT_BIT)
#define TCN75_ONESHOT	  (1U << TCN75_ONESHOT_BIT)

#define TCN75_DEFAULT_CONFIG                                                     \
	(TCN75_NORMAL_OPERATION | TCN75_COMPARATOR_MODE |                        \
	 TCN75_COMPINT_POLARITY_ACTIVE_LOW | TCN75_FAULT_QUEUE_NB_CONVERSION_6 | \
	 TCN75_RESOLUTION_12BIT | TCN75_CONTINUOUS)

/**
 * @brief TCN75 context
 */
struct tcn75_context {
	/* TCN75 device address, of the form TCN75_ADDR_BASE + 0b000_0xxx */
	uint8_t addr;
	/* TCN75 configuration register, e.g. TCN75_DEFAULT_CONFIG */
	uint8_t config;
	/* I2C device the TCN75 is connected to */
	I2C_Device *i2c;
};

/**
 * @brief Initialize a TCN75 context.
 *
 * @param tcn75 TCN75 context to initialize
 * @param addr Address of the device
 * @param config TCN75 configuration register
 * @param i2c I2C device the TCN75 is connected to
 * @return int8_t 0 if success, negative value otherwise
 */
int8_t tcn75_init_context(struct tcn75_context *tcn75,
			  uint8_t addr,
			  uint8_t config,
			  I2C_Device *i2c);

/**
 * @brief Configure a TCN75 device using its configuration register.
 *
 * @param tcn75 initialized TCN75 context
 * @return int8_t 0 if success, negative value otherwise
 */
int8_t tcn75_configure(struct tcn75_context *tcn75);

/**
 * @brief Select the temperature register of a TCN75 device.
 *
 * @param tcn75 initialized TCN75 context
 * @return int8_t 0 if success, negative value otherwise
 */
int8_t tcn75_select_data_register(struct tcn75_context *tcn75);

/**
 * @brief Read the temperature from a TCN75 device.
 *
 * @param tcn75 initialized TCN75 context
 * @return int16_t temperature in 0.01°C resolution, returns INT16_MAX if error

 */
int16_t tcn75_read(struct tcn75_context *tcn75);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_DEVICE_TCN75_H */