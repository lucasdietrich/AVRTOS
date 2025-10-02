/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdout.h"

#include "misc/serial.h"

#if CONFIG_STDIO_USART == 0

/**
 * @brief Custom character output function for USART.
 *
 * @param c The character to be transmitted.
 * @param stream The stream to which the character is written (not used).
 * @return 0 on success.
 */
static int uart_putchar(char c, FILE *stream)
{
    (void)stream;
    serial_transmit(c);
    return 0;
}

/**
 * @brief Custom character input function for USART.
 *
 * @param stream The stream from which the character is read (not used).
 * @return The received character.
 */
static int uart_getchar(FILE *stream)
{
    (void)stream;
    return serial_receive();
}

/**
 * @brief File stream for USART output.
 *
 * This file stream is used to direct standard input and output to configured serial
 * USART.
 */
static FILE z_serial_file = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

/**
 * @brief Set standard I/O to use serial USART.
 *
 * This function configures the standard I/O library to use USART for
 * output operations. It sets `stdout` to the `z_serial_file` file stream
 * which directs output to USART.
 */
void k_set_stdio_serial(void)
{
    stdout = &z_serial_file;
    stdin  = &z_serial_file;
}

#elif CONFIG_STDIO_USART > 0

#warning "CONFIG_STDIO_USART > 0 not supported"

#endif /* CONFIG_STDIO_USART */
