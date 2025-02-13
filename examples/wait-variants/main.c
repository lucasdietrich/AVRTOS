/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <util/delay.h>

int main(void)
{
	serial_init();

	k_thread_dump_all();

	k_show_uptime();
	printf_P(PSTR("Startup\n"));

	for (;;) {
		k_show_uptime();
		serial_transmit('\n');

		k_sleep(K_SECONDS(1u));
		k_wait(K_SECONDS(1u), K_WAIT_MODE_IDLE);
		k_wait(K_SECONDS(1u), K_WAIT_MODE_ACTIVE);
		k_wait(K_SECONDS(1u), K_WAIT_MODE_BLOCK);

		z_cpu_block_ms(2);
		z_cpu_block_us(100);
	}
}