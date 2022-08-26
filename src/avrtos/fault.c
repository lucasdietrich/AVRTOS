/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fault.h"

#include "misc/uart.h"

#include <avr/pgmspace.h>

void __fault(uint8_t reason)
{
	cli();

	usart_print_p(PSTR("***** Kernel Fault *****"));
	usart_print_p(PSTR("\r\nreason="));
	usart_hex(reason);
	usart_print_p(PSTR("\tth: 0x"));
	usart_hex16((const uint16_t) _current);

	asm("call __debug");

	asm("jmp _exit");

	__builtin_unreachable();
}