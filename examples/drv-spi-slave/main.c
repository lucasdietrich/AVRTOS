/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/avrtos.h>
#include <avrtos/drivers/spi.h>
#include <avrtos/drivers/exti.h>

/* Button should be wired to PD0 (INT0) on Arduino Mega 2560,
 * with a pull-up resistor.
 */
#define USE_BUTTON 0u

/** Enable/disable data rate calculation, has a significant impact on performance */
#define MEASURE_STATS 1u

#if MEASURE_STATS
volatile uint32_t rx_count = 0u;
volatile uint16_t err_count = 0u;
#endif

bool spi_callback(char *rxtx)
{
	static uint8_t last = 0u;
	/* Read rxtx and prepare next byte */
	__Z_DBG_GPIO_0_TOGGLE();

#if MEASURE_STATS
	rx_count++;
	if ((uint8_t)(last - *rxtx + 1u)) {
		err_count++;
	}
	last = *rxtx;
#endif

	return true;
}

#if USE_BUTTON
bool spi_enabled = false;

struct k_signal sig;

ISR(INT0_vect)
{
	int ret = k_signal_raise(&sig, 1u);

	/* Yield if more than one thread was woken up */
	if (ret > 0) k_yield_from_isr();
}

#endif

int main(void)
{
	serial_init();

	/* Advised configuration */
	const struct spi_config cfg = {
		.mode	     = SPI_MODE_SLAVE,
		.polarity    = SPI_CLOCK_POLARITY_RISING,
		.phase	     = SPI_CLOCK_PHASE_SAMPLE,
		.irq_enabled = 0u,
	};

	spi_init(cfg);

#if USE_BUTTON
	k_signal_init(&sig);

	gpiol_pin_init(GPIOD, PIN0, GPIO_INPUT, PIN_NO_PULLUP);

	/* Configure INT0 to trigger on falling edge */
	exti_configure(INT0, ISC_FALLING);
	exti_clear_flag(INT0);
	exti_enable(INT0);

#endif

	k_thread_dump_all();

#if USE_BUTTON
	printf_P(PSTR("SPI Slave ready, use button to enabled/disable SPI\n"));

	for (;;) {
		k_poll_signal(&sig, K_FOREVER);

		spi_enabled = !spi_enabled;

		printf_P(PSTR("Button pressed\n"));

		if (spi_enabled) {
			printf_P(PSTR("SPI enabled\n"));
			spi_transceive_async_start(0xAA, spi_callback);
		} else {
			printf_P(PSTR("SPI disabled\n"));
			spi_cancel_async();
		}

		K_SIGNAL_SET_UNREADY(&sig);
	}
#else
	spi_transceive_async_start(0xAA, spi_callback);

	k_sleep(K_FOREVER);
#endif
}

#if MEASURE_STATS
void calculate_datarate(void *arg)
{
	uint32_t last, now, diff;
	uint32_t rx;
	uint16_t er;

	last = k_uptime_get_ms32();

	for (;;) {
		k_sleep(K_SECONDS(1));

		now  = k_uptime_get_ms32();
		diff = now - last;
		last = now;

		irq_disable();
		rx	 = rx_count;
		rx_count = 0u;
		er	 = err_count;
		err_count = 0u;
		irq_enable();

		const uint32_t dr_bps = rx * 1000lu / diff;
		const uint16_t dr_kbps = dr_bps / 1000lu;
		const uint16_t dr_kbps_frac = dr_bps % 1000lu;

		printf_P(PSTR("Data rate: %u.%03u B/s err: %u (%u %%)\n"), dr_kbps,
			 dr_kbps_frac, er, er * 100lu / rx);
	}
}

K_THREAD_DEFINE(dr, calculate_datarate, 0x100, K_PREEMPTIVE, NULL, 'D');
#endif