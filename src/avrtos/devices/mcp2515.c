/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mcp2515.h"
#include <stdint.h>
#include <string.h>

// Intruction set (TABLE 12-1: SPI INSTRUCTION SET)
#define MCP_RESET				0xC0
#define MCP_READ				0x03
#define MCP_READ_RX_BUFFER(nm)	(0x90 | (nm << 1))
#define MCP_WRITE				0x02
#define MCP_LOAD_TX_BUFFER(abc) (0x40 | (abc))
#define MCP_RTS(nnn)			(0x80 | (nnn))
#define MCP_READ_STATUS			0xA0
#define MCP_RX_STATUS			0xB0
#define MCP_BIT_MODIFY			0x05

#define MCP_REG_CANSTAT 0x0E

#define MCP_RX0IF  0x01
#define MCP_RX1IF  0x02
#define MCP_TX0REQ 0x04
#define MCP_TX0IF  0x08
#define MCP_TX1REQ 0x10
#define MCP_TX1IF  0x20
#define MCP_TX2REQ 0x40
#define MCP_TX2IF  0x80

#define MCP_MASK_MODE 0xE0

#define MCP_MODE_NORMAL		 0x00
#define MCP_MODE_SLEEP		 0x20
#define MCP_MODE_LOOPBACK	 0x40
#define MCP_MODE_LISTEN_ONLY 0x60
#define MCP_MODE_CONFIG		 0x80

#define MCP_SELECT(_dev)   spi_slave_select(&_dev->slave)
#define MCP_UNSELECT(_dev) spi_slave_unselect(&_dev->slave)
#define spi_read()		   spi_transceive(0x00)

static int8_t read_register(struct mcp2515_device *dev, uint8_t reg_addr)
{
	MCP_SELECT(dev);
	spi_transceive(MCP_READ);
	spi_transceive(reg_addr);
	uint8_t reg_val = spi_transceive(0x00);
	MCP_UNSELECT(dev);

	return reg_val;
}

static void
read_registers(struct mcp2515_device *dev, uint8_t reg_addr, uint8_t *buf, size_t len)
{
	MCP_SELECT(dev);
	spi_transceive(MCP_READ);
	spi_transceive(reg_addr);
	while (len--) {
		*buf++ = spi_transceive(0x00);
	}
	MCP_UNSELECT(dev);
}

static void write_register(struct mcp2515_device *dev, uint8_t reg_addr, uint8_t reg_val)
{
	MCP_SELECT(dev);
	spi_transceive(MCP_WRITE);
	spi_transceive(reg_addr);
	spi_transceive(reg_val);
	MCP_UNSELECT(dev);
}

static void write_registers(struct mcp2515_device *dev,
							uint8_t reg_addr,
							const uint8_t *buf,
							size_t len)
{
	MCP_SELECT(dev);
	spi_transceive(MCP_WRITE);
	spi_transceive(reg_addr);
	while (len--) {
		spi_transceive(*buf++);
	}
	MCP_UNSELECT(dev);
}

static void
modify_register(struct mcp2515_device *dev, uint8_t reg_addr, uint8_t mask, uint8_t data)
{
	MCP_SELECT(dev);
	spi_transceive(MCP_BIT_MODIFY);
	spi_transceive(reg_addr);
	spi_transceive(mask);
	spi_transceive(data);
	MCP_UNSELECT(dev);
}

uint8_t get_mode(struct mcp2515_device *dev)
{
	return read_register(dev, MCP_REG_CANSTAT) & MCP_MASK_MODE;
}

uint8_t mcp2515_read_status(struct mcp2515_device *dev)
{
	MCP_SELECT(dev);
	spi_transceive(MCP_READ_STATUS);
	uint8_t status = spi_transceive(0x00);
	MCP_UNSELECT(dev);
	return status;
}

int8_t msp2515_check_msg(struct mcp2515_device *dev)
{
	uint8_t status = mcp2515_read_status(dev);
	return (status & (MCP_RX0IF | MCP_RX1IF)) ? MCP2515_MSG_AVAILABLE : MCP2515_NO_MSG;
}

void mcp2515_reset(struct mcp2515_device *dev)
{
	MCP_SELECT(dev);
	spi_transceive(MCP_RESET);
	MCP_UNSELECT(dev);
}

#define MCP_16MHz_500kBPS_CFG1 (0x00)
#define MCP_16MHz_500kBPS_CFG2 (0xF0)
#define MCP_16MHz_500kBPS_CFG3 (0x86)

#define MCP_CNF3 0x28
#define MCP_CNF2 0x29
#define MCP_CNF1 0x2A

int8_t mcp2515_config_dr(struct mcp2515_device *dev,
						 mcp2515_can_speed_t speed,
						 mcp2515_clock_set_t clock)
{
	// MCP_16MHz and CAN_500KBPS
	if (speed != MCP2515_CAN_SPEED_500KBPS || clock != MCP2515_CLOCK_SET_16MHZ) {
		return -ENOTSUP;
	}

	uint8_t cfg1 = MCP_16MHz_500kBPS_CFG1;
	uint8_t cfg2 = MCP_16MHz_500kBPS_CFG2;
	uint8_t cfg3 = MCP_16MHz_500kBPS_CFG3;

	write_register(dev, MCP_CNF1, cfg1);
	write_register(dev, MCP_CNF2, cfg2);
	write_register(dev, MCP_CNF3, cfg3);

	return 0;
}

int8_t mcp2515_init(struct mcp2515_device *dev,
					const struct mcp2515_config *config,
					struct spi_slave *spi_slave)
{
	int8_t ret;

	// Init SPI
	spi_regs_restore(&spi_slave->regs);
	spi_slave_ss_init(&dev->slave);

	memcpy(&dev->slave, spi_slave, sizeof(struct spi_slave));
	k_mutex_init(&dev->_mutex);

	// Init interrupt pin

	// Reset MCP
	mcp2515_reset(dev);
	k_msleep(10);

	ret = mcp2515_config_dr(dev, config->can_speed, config->clock_speed);

	return ret;
}