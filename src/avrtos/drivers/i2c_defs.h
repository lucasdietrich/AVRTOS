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

#endif /* DRIVERS_SPI_DEFS_H_ */