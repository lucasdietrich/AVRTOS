/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_SUBSYS_CANQ
#define _AVRTOS_SUBSYS_CANQ

#include <stddef.h>
#include <stdint.h>

#include <avrtos/drivers/can.h>

#define K_CAN_MSGQ_DEFINE(_name, _max_msgs)                                              \
	K_MSGQ_DEFINE(_name, sizeof(struct can_frame), _max_msgs)

struct can_rxq {
	struct can_filter filter;
	struct k_msgq *msgq;
};

#endif /* _AVRTOS_DRIVERS_CAN */