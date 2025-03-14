/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_I2C_DEFS_H_
#define DRIVERS_I2C_DEFS_H_

#include <avr/io.h>

#if !defined(PRR0)
#define PRR0 PRR
#endif

#if !defined(PRTWI0)
#define PRTWI0 PRTWI
#endif

#if !defined(TWI0_vect)
#define TWI0_vect TWI_vect
#endif

#if defined(TWI1_vect)
#define MCU_I2C_COUNT 2
#elif defined(TWI0_vect)
#define MCU_I2C_COUNT 1
#else
#define MCU_I2C_COUNT 0
#endif

#define AVR_I2C_BASE_ADDR     (AVR_IO_BASE_ADDR + 0x00B8u)
#define AVR_I2Cn_BASE_ADDR(n) (AVR_I2C_BASE_ADDR + (n)*0x20)
#define AVR_I2Cn_DEVICE(n)    ((I2C_Device *)AVR_I2Cn_BASE_ADDR(n))

#define I2C_CONFIG_INIT(_prescaler, _twbr)                                               \
    (struct i2c_config)                                                                  \
    {                                                                                    \
        .prescaler = _prescaler, .twbr = _twbr                                           \
    }

#define I2C_PRESCALER_VALUE(_prescaler) (1 << (_prescaler << 1))

#define I2C_CALC_TWBR(_prescaler, _freq)                                                 \
    (((F_CPU / (_freq)) - 16) / (2 * I2C_PRESCALER_VALUE(_prescaler)))

#define I2C_CONFIG_INIT_FREQ(_prescaler, _freq)                                          \
    (struct i2c_config)                                                                  \
    {                                                                                    \
        .prescaler = _prescaler, .twbr = I2C_CALC_TWBR(_prescaler, _freq)                \
    }

#endif /* DRIVERS_SPI_DEFS_H_ */