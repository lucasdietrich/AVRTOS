/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "assert.h"

#include <avr/pgmspace.h>

#include "misc/serial.h"

void __assert(uint8_t expression, uint8_t module, uint8_t acode, uint16_t line)
{
	if (expression == 0u) {
		cli();

		serial_print_p(PSTR("\n\n\n**** K assert !!! ****\n"));

		serial_print_p(PSTR("mod=0x"));
		serial_hex(module);
		serial_print_p(PSTR(":L 0x"));
		serial_u16(line);
		serial_print_p(PSTR(" acode=0x"));
		serial_u16(acode);

		asm("jmp _exit");

		__builtin_unreachable();
	}
}