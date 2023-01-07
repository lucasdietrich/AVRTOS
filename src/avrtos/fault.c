/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fault.h"
#include "misc/serial.h"

#include <avr/pgmspace.h>

void __fault(uint8_t reason)
{
	cli();

	serial_print_p(PSTR("***** Kernel Fault *****"));
	serial_print_p(PSTR("\r\nreason="));
	serial_hex(reason);
	serial_print_p(PSTR("\tth: 0x"));
	serial_hex16((const uint16_t)z_current);

	asm("call __debug");

	asm("jmp _exit");

	__builtin_unreachable();
}