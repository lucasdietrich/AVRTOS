/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/spi.h>
#include <avrtos/avrtos.h>
#include <util/crc16.h>

int main(void)
{
	serial_init();

	const struct spi_config cfg = {
		.mode	     = SPI_MODE_MASTER,
		.polarity    = SPI_CLOCK_POLARITY_RISING,
		.phase	     = SPI_CLOCK_PHASE_SAMPLE,
		.prescaler   = SPI_PRESCALER_4,
		.irq_enabled = 0u,
	};

	gpiol_pin_init(GPIOB, PIN0, GPIO_MODE_OUTPUT, GPIO_HIGH);

	spi_init(&cfg);

	k_thread_dump_all();

	char chr = 0u;

	for (;;) {
		gpiol_pin_write_state(GPIOB, PIN0, GPIO_LOW);

		spi_transceive(chr);

		gpiol_pin_write_state(GPIOB, PIN0, GPIO_HIGH);

		__Z_DBG_GPIO_0_TOGGLE();

		k_block_us(10u);

		chr++;
	}
}