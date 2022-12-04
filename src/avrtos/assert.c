/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "assert.h"

#include <avr/pgmspace.h>

#include "misc/uart.h"

void __assert(uint8_t expression, uint8_t module, uint8_t acode, uint16_t line)
{
	if (expression == 0u) {
		cli();

		usart_print_p(PSTR("\n\n\n**** K assert !!! ****\n"));

		usart_print_p(PSTR("mod=0x"));
		usart_hex(module);
		usart_print_p(PSTR(":L 0x"));
		usart_u16(line);
		usart_print_p(PSTR(" acode=0x"));
		usart_u16(acode);

		asm("jmp _exit");

		__builtin_unreachable();
	}
}