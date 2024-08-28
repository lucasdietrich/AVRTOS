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
#include <avrtos/msgq.h>
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

typedef enum mcp2515_clock_set {
	MCP2515_CLOCK_SET_8MHZ = 0u,
	MCP2515_CLOCK_SET_16MHZ,
} mcp2515_clock_set_t;

struct mcp2515_config {
	mcp2515_can_speed_t can_speed : 3u;
	mcp2515_clock_set_t clock_speed : 1u;
};

struct mcp2515_device {
	struct spi_slave slave;
	struct k_mutex Z_PRIVATE(mutex);
};

#define MCP2515_OK				   0
#define MCP2515_INIT_ERROR		   1
#define MCP2515_SEND_ERROR		   2
#define MCP2515_MSG_AVAILABLE	   3
#define MCP2515_NO_MSG			   4
#define MCP2515_CTRL_ERROR		   5
#define MCP2515_GET_TX_BUF_TIMEOUT 6
#define MCP2515_SEND_MSG_TIMEOUT   7
#define MCP2515_ERR				   0xFF

#ifdef __cplusplus
extern "C" {
#endif

int8_t mcp2515_init(struct mcp2515_device *dev,
					const struct mcp2515_config *config,
					struct spi_slave *spi_slave);

void mcp2515_reset(struct mcp2515_device *dev);
uint8_t mcp2515_read_status(struct mcp2515_device *dev);
int8_t msp2515_check_msg(struct mcp2515_device *dev);

int8_t mcp2515_config_dr(struct mcp2515_device *dev,
						 mcp2515_can_speed_t speed,
						 mcp2515_clock_set_t clock);

int8_t mcp2515_deinit(struct mcp2515_device *dev);

int8_t mcp2515_send(struct mcp2515_device *dev, struct can_frame *frame);

int8_t
mcp2515_recv(struct mcp2515_device *dev, struct can_frame *frame, k_timeout_t timeout);

int8_t mcp2515_install_rx_msgq(struct mcp2515_device *dev,
							   struct can_filter *filter,
							   struct k_msgq *msgq);

int8_t msp2515_uninstall_rx_msgq(struct mcp2515_device *dev, struct k_msgq *msgq);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_DEVICE_MCP2515_H */