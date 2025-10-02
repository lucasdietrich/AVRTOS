/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVERS_H_
#define _AVRTOS_DRIVERS_H_

#include <stddef.h>

#include <avr/io.h>

#define __IO volatile
#define __I  volatile const
#define __O  volatile

#define AVR_IO_BASE_ADDR 0x0000u

#endif /* _AVRTOS_DRIVERS_H_ */