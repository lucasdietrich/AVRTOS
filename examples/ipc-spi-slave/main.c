/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/kernel.h>
#include <avrtos/drivers/spi.h>
#include <avrtos/drivers/exti.h>

bool spi_callback(char *rxtx)
{
	__Z_DBG_GPIO_0_TOGGLE();

	return true;
}

bool spi_enabled = false;

struct k_signal sig;

ISR(INT0_vect)
{
	struct k_thread *thread = k_signal_raise(&sig, 1u);

	k_yield_from_isr_cond(thread);
}

int main(void)
{
	serial_init();

	const struct spi_config cfg = {
		.mode	     = SPI_MODE_SLAVE,
		.polarity    = SPI_CLOCK_POLARITY_RISING,
		.phase	     = SPI_CLOCK_PHASE_SAMPLE,
		.irq_enabled = 0u,
	};

	spi_init(&cfg);

	k_signal_init(&sig);

	gpiol_pin_init(GPIOD, PIN0, GPIO_INPUT, PIN_NO_PULLUP);

	/* Configure INT0 to trigger on falling edge */
	exti_configure(INT0, ISC_FALLING);
	exti_clear_flag(INT0);
	exti_enable(INT0);

	k_thread_dump_all();

	for (;;) {
		k_poll_signal(&sig, K_FOREVER);

		spi_enabled = !spi_enabled;

		printf("Button pressed\n");

		if (spi_enabled) {
			spi_tranceive_async(0xAA, spi_callback);
		} else {
			spi_cancel_async();
		}

		K_SIGNAL_SET_UNREADY(&sig);
	}
}