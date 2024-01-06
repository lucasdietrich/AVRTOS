/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_I2C_H_
#define DRIVERS_I2C_H_


#include <avrtos/kernel.h>
#include <avrtos/semaphore.h>
#include <avrtos/drivers.h>
#include <avrtos/drivers/gpio.h>

#include "i2c_defs.h"

/**
 * I2C driver
 *
 */

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
	__IO uint8_t TWBRn;  /* Bit rate register B8 */
	__IO uint8_t TWSRn;  /* Status register B9 */
	__IO uint8_t TWARn;  /* Address register BA*/
	__IO uint8_t TWDRn;  /* Address/Data Shift register BB */
	__IO uint8_t TWCRn;  /* Control register BC */
	__IO uint8_t TWAMRn; /* (Master mode) Address mask register BD */
} I2C_Device;

#define AVR_I2C_BASE_ADDR     (AVR_IO_BASE_ADDR + 0x00B8u)
#define AVR_I2Cn_BASE_ADDR(n) (AVR_I2C_BASE_ADDR + (n)*0x20)
#define AVR_I2Cn_DEVICE(n)    ((I2C_Device *)AVR_I2Cn_BASE_ADDR(n))

#define I2C0_BASE_ADDR AVR_I2Cn_BASE_ADDR(0)
#define I2C0_DEVICE    AVR_I2Cn_DEVICE(0)
#define I2C0	       I2C0_DEVICE
#define I2C0_INDEX     0

#define I2C_BASE_ADDR I2C0_BASE_ADDR
#define I2C_DEVICE    I2C0_DEVICE
#define I2C	      I2C_DEVICE
#define I2C_INDEX     I2C0_INDEX

// header iom328pb.h is missing in avr toolchain, need to be added manually as described here:
// https://stuvel.eu/post/2021-04-27-atmega328pb-on-gcc/
#if defined(TWI1_vect)
#define I2C1_BASE_ADDR AVR_I2Cn_BASE_ADDR(1)
#define I2C1_DEVICE    AVR_I2Cn_DEVICE(1)
#define I2C1	       I2C1_DEVICE
#define I2C1_INDEX     1
#endif

typedef enum {
	I2C_PRESCALER_1	 = 0u,
	I2C_PRESCALER_4	 = 1u,
	I2C_PRESCALER_16 = 2u,
	I2C_PRESCALER_64 = 3u,
} i2c_prescaler_t;

struct i2c_config {
	i2c_prescaler_t prescaler : 2;
};

typedef enum i2c_error {
	I2C_ERROR_NONE = 0u,
	I2C_ERROR_BUS  = 1u,
	I2C_ERROR_ADDR = 2u,
	I2C_ERROR_DATA = 3u,
	I2C_ERROR_ARGS = 4u,
} i2c_error_t;

int8_t i2c_init(I2C_Device *dev, struct i2c_config config);
void i2c_deinit(I2C_Device *dev);

int8_t i2c_master_transmit(I2C_Device *dev, uint8_t addr, const uint8_t *data, uint8_t len);
int8_t i2c_master_receive(I2C_Device *dev, uint8_t addr, uint8_t *data, uint8_t len);

int8_t i2c_status(I2C_Device *dev);
i2c_error_t i2c_poll_end(I2C_Device *dev);
i2c_error_t i2c_last_error(I2C_Device *dev);
struct k_sem *i2c_get_sem(I2C_Device *dev);

#if defined(__cplusplus)
}
#endif

#endif /* DRIVERS_I2C_H_ */