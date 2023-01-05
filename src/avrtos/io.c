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

static FILE k_stdout_usart0 = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void k_set_stdio_usart0(void)
{
	stdout = &k_stdout_usart0;
}

#else

void k_set_stdio_usart0(void)
{
	/* Nothing to do */
}

#endif /* CONFIG_STDIO_PRINTF_TO_USART */