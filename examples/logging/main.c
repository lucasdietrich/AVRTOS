/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/drivers/usart.h>
#include <avrtos/kernel.h>
#include <avrtos/logging.h>
#define LOG_LEVEL LOG_LEVEL_INF

int main(void)
{
	/* UART initialisation */
	const struct usart_config usart_config = {
		.baudrate    = USART_BAUD_500000,
		.receiver    = 0u,
		.transmitter = 1u,
		.mode	     = USART_MODE_ASYNCHRONOUS,
		.parity	     = USART_PARITY_NONE,
		.stopbits    = USART_STOP_BITS_1,
		.databits    = USART_DATA_BITS_8,
		.speed_mode  = USART_SPEED_MODE_NORMAL,
	};
	ll_usart_init(USART0_DEVICE, &usart_config);

	for (;;) {
		LOG_DBG("<dbg> Hello world!");
		LOG_INF("<inf> Hello world!");
		LOG_WRN("<wrn> Hello world!");
		LOG_ERR("<err> Hello world!");
		k_sleep(K_SECONDS(1));
	}
}
