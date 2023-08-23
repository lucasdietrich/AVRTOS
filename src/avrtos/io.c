/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "io.h"

#include "misc/serial.h"

#if CONFIG_STDIO_PRINTF_TO_USART == 0

static int uart_putchar(char c, FILE *stream)
{
	serial_transmit(c);
	return 0;
}

static FILE z_stdout_usart = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void k_set_stdio_usart0(void)
{
	stdout = &z_stdout_usart;
}

#elif CONFIG_STDIO_PRINTF_TO_USART > 0

#warning "CONFIG_STDIO_PRINTF_TO_USART > 0 not supported"

#endif /* CONFIG_STDIO_PRINTF_TO_USART */