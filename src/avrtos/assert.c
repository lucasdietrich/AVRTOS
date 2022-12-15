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

		/* IMPORTANT, do never use three succeeding exclamation points
		 * in a PGM string. This will cause the upload to fail.
		 * there are problem during the upload with avrdude. Maybe the 
		 * pattern is interpreted ?
		 * I have no clue, and don't want to debug this
		 * 
		 * > !!!
		 * 
		 */
		serial_print_p(PSTR("\n\n\n*** K assert ! ***\n"));

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