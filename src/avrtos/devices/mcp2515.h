/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEVICE_MCP2515_H
#define _AVRTOS_DEVICE_MCP2515_H

#include <stddef.h>
#include <stdint.h>

#include <avrtos/drivers/can.h>
#include <avrtos/drivers/spi.h>
#include <avrtos/kernel.h>
#include <avrtos/mutex.h>

typedef enum mcp2515_can_speed {
	MCP2515_CAN_SPEED_10KBPS  = 0u,
	MCP2515_CAN_SPEED_20KBPS  = 1u,
	MCP2515_CAN_SPEED_50KBPS  = 2u,
	MCP2515_CAN_SPEED_100KBPS = 3u,
	MCP2515_CAN_SPEED_125KBPS = 4u,
	MCP2515_CAN_SPEED_250KBPS = 5u,
	MCP2515_CAN_SPEED_500KBPS = 6u,
	MCP2515_CAN_SPEED_1MBPS	  = 7u,
} mcp2515_can_speed_t;

typedef enum mcp2515_flags {
	/* Enable interrupt on full RX buffer */
	MCP2515_INT_RX = BIT(0u),
	/* Enable interrupt on empty TX buffer */
	MCP2515_INT_TX = BIT(1u),
	/* Disable rollover (for debug purpose only)*/
	MCP2515_DEBUG_NO_ROLLOVER = BIT(2u),
	/* Disable filtering (for debug purpose only)*/
	MCP2515_DEBUG_RX_ANY = BIT(3u),
} mcp2515_flags_t;

typedef enum mcp2515_clock_set {
	MCP2515_CLOCK_SET_8MHZ = 0u,
	MCP2515_CLOCK_SET_16MHZ,
} mcp2515_clock_set_t;

struct mcp2515_config {
	/* CAN speed */
	mcp2515_can_speed_t can_speed : 3u;

	/* MCP2515 clock */
	mcp2515_clock_set_t clock_speed : 1u;

	/* MCP2515 flags */
	uint8_t flags : 4u;
};

struct mcp2515_device {
	struct spi_slave slave;
	struct k_mutex Z_PRIVATE(mutex);
};

#ifdef __cplusplus
extern "C" {
#endif

int8_t mcp2515_init(struct mcp2515_device *mcp,
					const struct mcp2515_config *config,
					struct spi_slave *spi_slave);

int8_t mcp2515_deinit(struct mcp2515_device *mcp);

int8_t mcp2515_send(struct mcp2515_device *mcp, const struct can_frame *frame);

int8_t mcp2515_recv(struct mcp2515_device *mcp, struct can_frame *frame);

int8_t mcp2515_set_filter(struct mcp2515_device *mcp,
						  uint8_t index,
						  uint8_t is_ext,
						  uint32_t filter);

int8_t mcp2515_set_mask(struct mcp2515_device *mcp,
						uint8_t index,
						uint8_t is_ext,
						uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_DEVICE_MCP2515_H */