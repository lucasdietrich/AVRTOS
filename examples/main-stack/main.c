/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/drivers/usart.h>
#include <avrtos/avrtos.h>

#include <avr/io.h>

__noinit char c;

extern char inc(char c);

int main(void)
{
	const struct usart_config usart_config = {
		.baudrate    = USART_BAUD_500000,
		.receiver    = 1u,
		.transmitter = 1u,
		.mode	     = USART_MODE_ASYNCHRONOUS,
		.parity	     = USART_PARITY_NONE,
		.stopbits    = USART_STOP_BITS_1,
		.databits    = USART_DATA_BITS_8,
		.speed_mode  = USART_SPEED_MODE_NORMAL,
	};
	ll_usart_init(USART0_DEVICE, &usart_config);

	for (;;) {
		ll_usart_sync_putc(USART0_DEVICE, inc(c));
		z_cpu_block_ms(1000u);
	}
}