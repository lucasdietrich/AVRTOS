/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/kernel.h>

#include <Arduino.h>
#include <SPI.h>

int main(void)
{
	serial_init();

	pinMode(8, OUTPUT);
	digitalWrite(8, HIGH);

	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV128);

	k_thread_dump_all();

	uint8_t tx = 0u;
	uint8_t rx;

	for (;;) {
		k_show_uptime();

		digitalWrite(8, LOW);
		

		digitalWrite(8, HIGH);
		
		printf_P(PSTR("SPI tx: %02x rx: %02x\n"), tx, rx);

		tx++;

		k_sleep(K_MSEC(1u));
	}
}