/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DRIVERS_CAN
#define _AVRTOS_DRIVERS_CAN

#include <stddef.h>
#include <stdint.h>

#include <avrtos/drivers.h>

typedef struct can_filter {
	uint32_t id;
	uint32_t mask;
} can_filter_t;

typedef struct can_frame {
	uint8_t data[8u];
	uint8_t len;
	uint32_t id : 29u;
	uint32_t rtr : 1u;
	uint32_t is_ext : 1u;
} __attribute__((packed)) can_frame_t;

#endif /* _AVRTOS_DRIVERS_CAN */