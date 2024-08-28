/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/spi.h>
#include <avrtos/drivers/can.h>
#include <avrtos/devices/mcp2515.h>

int main(void)
{
	serial_init_baud(115200u);

	const struct spi_config cfg = {
		.mode		 = SPI_MODE_MASTER,
		.polarity	 = SPI_CLOCK_POLARITY_FALLING,
		.phase		 = SPI_CLOCK_PHASE_SAMPLE,
		.prescaler	 = SPI_PRESCALER_4,
		.irq_enabled = 0u,
	};

	struct spi_slave spi_slave = {
		.cs_port	  = GPIOB_DEVICE,
		.cs_pin		  = 2u,
		.active_state = GPIO_LOW,
		.regs		  = spi_config_into_regs(cfg),
	};

	const struct mcp2515_config mcp2515_cfg = {
		.can_speed = MCP2515_CAN_SPEED_500KBPS,
		.clock_speed = MCP2515_CLOCK_SET_16MHZ,
	};

	spi_init(cfg);

	struct mcp2515_device mcp2515_dev;
	mcp2515_init(&mcp2515_dev, &mcp2515_cfg, &spi_slave);

	uint16_t i = 0;
	for (;;) {
		mcp2515_reset(&mcp2515_dev);
		uint8_t status = mcp2515_read_status(&mcp2515_dev);
		printf("%u: MCP2515 status: 0x%02X\n", i++, status);
		k_sleep(K_SECONDS(1));
	}
}