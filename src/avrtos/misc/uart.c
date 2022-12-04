/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "uart.h"

#include <avrtos/drivers/usart.h>
#include <avr/pgmspace.h>

#define USART_DEVICE USART0_DEVICE

void usart_init()
{
	/* UART initialisation */
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
	usart_ll_drv_init(USART_DEVICE, &usart_config);
}

void usart_transmit(char data)
{
	usart_ll_drv_sync_putc(USART_DEVICE, data);
}

void usart_send(const char *buffer, size_t len)
{
        for (uint_fast8_t i = 0; i < len; i++) {
                usart_transmit(buffer[i]);
        }
}

static char figure2hex(uint8_t value)
{
	return (value < 10) ? value + '0' : value + 'A' - 10;
}

void usart_u8(const uint8_t val)
{
        const char hundred = figure2hex(val / 100);
        const char ten = figure2hex((val / 10) % 10);
        const char unit = figure2hex(val % 10);

        if (val >= 100) {
                usart_transmit(hundred);
        }
        if (val >= 10) {
                usart_transmit(ten);
        }

        usart_transmit(unit);
}

void  usart_u16(uint16_t val)
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

        usart_send(&digits[first_digit], 5 - first_digit);
}

void usart_s8(const int8_t val)
{
        uint8_t u8_val;

        if (val == 0) {
                usart_transmit('0');

                return;
        } else if (val < 0) {
                u8_val = (uint8_t)(-val);
                usart_transmit('-');
        } else {
                u8_val = (uint8_t)(val);
        }
        usart_u8(u8_val);
}

void usart_hex(const uint8_t val)
{
        const char high = figure2hex(val >> 4);
        const char low = figure2hex(val & 0xF);

        usart_transmit(high);
        usart_transmit(low);
}

void usart_hex16(const uint16_t val)
{
        usart_hex((uint8_t)(val >> 8));
        usart_hex((uint8_t)val);
}

void usart_send_hex(const uint8_t *buffer, size_t len)
{
        for (uint_fast8_t i = 0; i < len; i++) {
                usart_hex(buffer[i]);

                if (0xF == (i & 0xF)) {
                        usart_transmit('\n'); // EOL every 16 chars
                } else {
                        usart_transmit(' ');
                }
        }
}

void usart_print(const char *text)
{
        usart_send(text, strlen(text));
}

void usart_printl(const char *text)
{
        usart_print(text);
        usart_transmit('\n');
}

void usart_send_p(const char *buffer, size_t len)
{
        for (uint_fast16_t i = 0; i < len; i++) {
                usart_transmit(pgm_read_byte(&buffer[i]));
        }
}

void usart_print_p(const char *text)
{
        usart_send_p(text, strlen_P(text));
}

void usart_printl_p(const char *text)
{
        usart_print_p(text);
        usart_transmit('\n');
}