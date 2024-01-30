/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVERS_I2C_H_
#define _AVRTOS_DRIVERS_I2C_H_

#include <avrtos/drivers.h>
#include <avrtos/kernel.h>

#include "i2c_defs.h"

/**
 * I2C driver
 */

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
	__IO uint8_t TWBRn;	 /* Bit rate register B8 */
	__IO uint8_t TWSRn;	 /* Status register B9 */
	__IO uint8_t TWARn;	 /* Address register BA*/
	__IO uint8_t TWDRn;	 /* Address/Data Shift register BB */
	__IO uint8_t TWCRn;	 /* Control register BC */
	__IO uint8_t TWAMRn; /* (Master mode) Address mask register BD */
} I2C_Device;

#define I2C0_BASE_ADDR AVR_I2Cn_BASE_ADDR(0)
#define I2C0_DEVICE	   AVR_I2Cn_DEVICE(0)
#define I2C0		   I2C0_DEVICE

#define I2C_BASE_ADDR I2C0_BASE_ADDR
#define I2C_DEVICE	  I2C0_DEVICE
#define I2C			  I2C_DEVICE

// header iom328pb.h is missing in avr toolchain, need to be added manually as
// described here: https://stuvel.eu/post/2021-04-27-atmega328pb-on-gcc/
#if defined(TWI1_vect)
#define I2C1_BASE_ADDR AVR_I2Cn_BASE_ADDR(1)
#define I2C1_DEVICE	   AVR_I2Cn_DEVICE(1)
#define I2C1		   I2C1_DEVICE
#endif

typedef enum {
	I2C_PRESCALER_1	 = 0u,
	I2C_PRESCALER_4	 = 1u,
	I2C_PRESCALER_16 = 2u,
	I2C_PRESCALER_64 = 3u,
} i2c_prescaler_t;

// standard I2C frequencies for 16MHz clock
#if F_CPU == 16000000
#define I2C_CONF_1600	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_64, 1600)
#define I2C_CONF_8000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_4, 8000)
#define I2C_CONF_10000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_4, 10000)
#define I2C_CONF_12500	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_4, 12500)
#define I2C_CONF_15625	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_4, 15625)
#define I2C_CONF_16000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_4, 16000)
#define I2C_CONF_20000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_4, 20000)
#define I2C_CONF_25000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_4, 25000)
#define I2C_CONF_31250	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 31250)
#define I2C_CONF_32000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 32000)
#define I2C_CONF_40000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 40000)
#define I2C_CONF_50000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 50000)
#define I2C_CONF_62500	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 62500)
#define I2C_CONF_64000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 64000)
#define I2C_CONF_80000	I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 80000)
#define I2C_CONF_100000 I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 100000)
#define I2C_CONF_125000 I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 125000)
#define I2C_CONF_160000 I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 160000)
#define I2C_CONF_200000 I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 200000)
#define I2C_CONF_250000 I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 250000)
#define I2C_CONF_320000 I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 320000)
#define I2C_CONF_400000 I2C_CONFIG_INIT_FREQ(I2C_PRESCALER_1, 400000)
#endif // F_CPU == 16000000

#define I2C_CONF_DEFAULT I2C_CONF_100000

/**
 * @brief Configuration structure for I2C
 */
struct i2c_config {
	/* I2C clock prescaler */
	i2c_prescaler_t prescaler : 2u;
	/* I2C bit rate register value */
	uint8_t twbr;
};

/**
 * @brief I2C driver error codes
 */
typedef enum i2c_error {
	/* No error */
	I2C_ERROR_NONE = 0u,
	/* Bus error (or unknown error) */
	I2C_ERROR_BUS = 1u,
	/* Address error, no slave responded or NACK received to address */
	I2C_ERROR_ADDR = 2u,
	/* Data error, NACK received */
	I2C_ERROR_DATA = 3u,
	/* Invalid arguments provided */
	I2C_ERROR_ARGS = 4u,
} i2c_error_t;

/**
 * @brief Initialize I2C driver
 *
 * @param dev Device to initialize
 * @param config Configuration structure
 * @return int8_t 0 if success, negative value otherwise
 */
int8_t i2c_init(I2C_Device *dev, struct i2c_config config);

/**
 * @brief Deinitialize I2C driver
 *
 * @param dev Device to deinitialize
 * @return int8_t 0 if success, negative value otherwise
 */
int8_t i2c_deinit(I2C_Device *dev);

/**
 * @brief Transmit data to I2C device as master
 *
 * The given buffer must remain valid until the transmission is complete.
 *
 * If CONFIG_I2C_BLOCKING option is enabled, this function will block until
 * the reception is complete, otherwise it will return immediately and the
 * user must poll for the end of transmission using i2c_poll_end().
 *
 * @param dev I2C device
 * @param addr Device address
 * @param data Data buffer to transmit
 * @param len Buffer length
 * @return int8_t Number of bytes written if success, negative value otherwise
 */
int8_t i2c_master_write(I2C_Device *dev, uint8_t addr, const uint8_t *data, uint8_t len);

/**
 * @brief Receive data from I2C device as master
 *
 * The given buffer must remain valid until the reception is complete.
 *
 * If CONFIG_I2C_BLOCKING option is enabled, this function will block until
 * the reception is complete, otherwise it will return immediately and the
 * user must poll for the end of transmission using i2c_poll_end().
 *
 * @param dev I2C device
 * @param addr Device address
 * @param data Data buffer to receive
 * @param len Buffer length
 * @return int8_t Number of bytes written if success, negative value otherwise
 */
int8_t i2c_master_read(I2C_Device *dev, uint8_t addr, uint8_t *data, uint8_t len);

/**
 * @brief Write w_len bytes to I2C device then read r_len bytes from it as master.
 *
 * This function is convenient for devices that require a write operation before
 * a read operation. It is equivalent to calling i2c_master_write() followed by
 * i2c_master_read() but it is more efficient as it does not require to wait for
 * the end of the first transmission before starting the second one.
 *
 * The given buffer must remain valid until the transmission is complete.
 *
 * If CONFIG_I2C_BLOCKING option is enabled, this function will block until
 * the reception is complete, otherwise it will return immediately and the
 * user must poll for the end of transmission using i2c_poll_end().
 *
 * @param dev I2C device
 * @param addr Device address
 * @param data Data buffer containing the wlen bytes to write, on function return
 * 				it will contain the rlen bytes read. Its size must be at least
 * 				the maximum between wlen and rlen.
 * @param wlen Number of bytes to write
 * @param rlen Number of bytes to read
 * @return int8_t Number of bytes written if success, negative value otherwise
 */
int8_t i2c_master_write_read(
	I2C_Device *dev, uint8_t addr, uint8_t *data, uint8_t wlen, uint8_t rlen);

/**
 * @brief Get I2C status
 *
 * @param dev I2C device
 * @return int8_t 0 if ready, -EBUSY if busy, negative value otherwise
 */
int8_t i2c_status(I2C_Device *dev);

/**
 * @brief Poll for I2C end of transmission
 *
 * Should be used when CONFIG_I2C_BLOCKING option is disabled.
 *
 * @param dev I2C device
 * @return i2c_error_t I2C_ERROR_NONE if success, negative value otherwise
 */
i2c_error_t i2c_poll_end(I2C_Device *dev);

/**
 * @brief Retrieve last I2C error
 *
 * Should be used when CONFIG_I2C_LAST_ERROR option is enabled.
 *
 * @param dev
 * @return i2c_error_t
 */
i2c_error_t i2c_last_error(I2C_Device *dev);

/**
 * @brief Calculate I2C configuration structure from desired frequency
 *
 * @param config Structure to fill
 * @param desired_freq Desired frequency
 * @return int8_t 0 if success, negative value otherwise
 */
int8_t i2c_calc_config(struct i2c_config *config, uint32_t desired_freq);

#if defined(__cplusplus)
}
#endif

#endif /* _AVRTOS_DRIVERS_I2C_H_ */