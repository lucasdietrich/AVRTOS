/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "serial.h"

#include <string.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avrtos/drivers/usart.h>

#define USART_DEVICE USART0_DEVICE

void serial_init_baud(uint32_t baud)
{
	/* UART initialisation */
	const struct usart_config usart_config = {
		.baudrate    = baud,
		.receiver    = 1u,
		.transmitter = 1u,
		.mode	     = USART_MODE_ASYNCHRONOUS,
		.parity	     = USART_PARITY_NONE,
		.stopbits    = USART_STOP_BITS_1,
		.databits    = USART_DATA_BITS_8,
		.speed_mode  = USART_SPEED_MODE_NORMAL,
	};
	ll_usart_init(USART_DEVICE, &usart_config);
}

void serial_transmit(char data)
{
	ll_usart_sync_putc(USART_DEVICE, data);
}

void serial_send(const char *buffer, size_t len)
{
	usart_send(USART_DEVICE, buffer, len);
}

static char figure2hex(uint8_t value)
{
	return (value < 10) ? value + '0' : value + 'A' - 10;
}

void serial_u8(const uint8_t val)
{
	const char hundred = figure2hex(val / 100);
	const char ten	   = figure2hex((val / 10) % 10);
	const char unit	   = figure2hex(val % 10);

	if (val >= 100) {
		serial_transmit(hundred);
	}
	if (val >= 10) {
		serial_transmit(ten);
	}

	serial_transmit(unit);
}

void serial_u16(uint16_t val)
{
	char digits[5];

	uint8_t first_digit = 4u;

	for (uint_fast8_t p = 0; p < 5; p++) {
		char cur = figure2hex(val % 10);

		val /= 10;

		digits[4u - p] = cur;

		if (cur != '0') {
			first_digit = 4u - p;
		}
	}

	serial_send(&digits[first_digit], 5 - first_digit);
}

void serial_s8(const int8_t val)
{
	uint8_t u8_val;

	if (val == 0) {
		serial_transmit('0');

		return;
	} else if (val < 0) {
		u8_val = (uint8_t)(-val);
		serial_transmit('-');
	} else {
		u8_val = (uint8_t)(val);
	}
	serial_u8(u8_val);
}

void serial_hex(const uint8_t val)
{
	const char high = figure2hex(val >> 4);
	const char low	= figure2hex(val & 0xF);

	serial_transmit(high);
	serial_transmit(low);
}

void serial_hex16(const uint16_t val)
{
	serial_hex((uint8_t)(val >> 8));
	serial_hex((uint8_t)val);
}

void serial_send_hex(const uint8_t *buffer, size_t len)
{
	for (uint_fast8_t i = 0; i < len; i++) {
		serial_hex(buffer[i]);

		if (0xF == (i & 0xF)) {
			serial_transmit('\n');	// EOL every 16 chars
		} else {
			serial_transmit(' ');
		}
	}
}

void serial_print(const char *text)
{
	serial_send(text, strlen(text));
}

void serial_printl(const char *text)
{
	serial_print(text);
	serial_transmit('\n');
}

void serial_send_p(const char *buffer, size_t len)
{
	for (uint_fast16_t i = 0; i < len; i++) {
		serial_transmit(pgm_read_byte(&buffer[i]));
	}
}

void serial_print_p(const char *text)
{
	serial_send_p(text, strlen_P(text));
}

void serial_printl_p(const char *text)
{
	serial_print_p(text);
	serial_transmit('\n');
}