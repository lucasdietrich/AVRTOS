/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>

#include <avrtos/kernel.h>
#include <avrtos/drivers/usart.h>

extern char inc(char c);

int main(void)
{
	const struct usart_config usart_config = {
		.baudrate = USART_BAUD_500000,
		.receiver = 1u,
		.transmitter = 1u,
		.mode = USART_MODE_ASYNCHRONOUS,
		.parity = USART_PARITY_NONE,
		.stopbits = USART_STOP_BITS_1,
		.databits = USART_DATA_BITS_8,
		.speed_mode = USART_SPEED_MODE_NORMAL
	};
	usart_ll_drv_init(USART0_DEVICE, &usart_config);

	for (;;) {
		usart_ll_drv_sync_putc(USART0_DEVICE, inc('a'));
		k_block(K_MSEC(1000u));
	}
}