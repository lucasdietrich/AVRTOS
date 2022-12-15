/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVERS_H_
#define _AVRTOS_DRIVERS_H_

#include <stddef.h>
#include <avr/io.h>

#define __IO 	volatile
#define __I 	volatile const
#define __O 	volatile

#define AVR_IO_BASE_ADDR 0x0000U

#endif /* _AVRTOS_DRIVERS_H_ */