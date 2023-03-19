/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fault.h"
#include "misc/serial.h"

#include <avr/pgmspace.h>

__attribute__((weak, used)) void __fault_hook(void)
{
}

void __fault(uint8_t reason)
{
	cli();

	/* Hook for debugging */
	asm("call __fault_hook");

#if !CONFIG_KERNEL_SILENT_FAULTS
	serial_print_p(PSTR("***** Kernel Fault *****"));
	serial_print_p(PSTR("\r\nreason="));
	serial_hex(reason);
	serial_print_p(PSTR("\tth: 0x"));
	serial_hex16((const uint16_t)z_current);
#endif /* CONFIG_KERNEL_SILENT_FAULTS */

	asm("jmp _exit");

	__builtin_unreachable();
}