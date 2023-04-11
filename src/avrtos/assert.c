/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "assert.h"
#include "misc/serial.h"

#include <avr/pgmspace.h>

extern uint8_t z_kernel_mode;

void z_assert_user_mode(void)
{
	__assert(!z_kernel_mode, K_MODULE_KERNEL, K_ASSERT_USER_MODE,
		 K_ASSERT_UNDEFINED_LINE);
}

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

		serial_print_p(PSTR("m=x"));
		serial_hex(module);
		serial_print_p(PSTR(" L="));
		serial_u16(line);
		serial_print_p(PSTR(" c="));
		serial_u16(acode);

		asm("jmp _exit");

		__builtin_unreachable();
	}
}