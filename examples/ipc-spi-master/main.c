/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/kernel.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/spi.h>
#include <avrtos/subsys/ipc/ipc.h>
#include <avrtos/logging.h>
#define LOG_LEVEL LOG_LEVEL_DEBUG

int main(void)
{
	int ret;

	serial_init();

	const struct spi_config spi_master_cfg = {
		.mode	     = SPI_MODE_MASTER,
		.polarity    = SPI_CLOCK_POLARITY_RISING,
		.phase	     = SPI_CLOCK_PHASE_SAMPLE,
		.prescaler   = SPI_PRESCALER_4,
		.irq_enabled = 0u,
	};

	const struct spi_slave spi_slave = {
		.cs_port = GPIOB_DEVICE,
		.cs_pin = PIN0,
		.active_state = GPIO_LOW,
	};

	const struct ipc_config ipc_cfg = {
	};

	ret = ipc_init(&ipc_cfg);
	LOG_DBG("ipc_init: %d", ret);

	for (;;) {

	}
}