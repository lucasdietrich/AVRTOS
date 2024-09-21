/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fault.h"

#include <avr/pgmspace.h>

#include "debug.h"
#include "misc/serial.h"

__attribute__((weak, used)) void __fault_hook(void)
{
}

void __fault(uint8_t reason)
{
	cli();

	/* Hook for debugging */
	asm("call __fault_hook");

#if CONFIG_KERNEL_FAULT_VERBOSITY == 1
	serial_print_p(PSTR("\n***** Kernel Fault *****"));
	serial_print_p(PSTR("\r\nreason="));
	serial_hex(reason);
	serial_print_p(PSTR("\tth: 0x"));
	serial_hex16((const uint16_t)z_ker.current);
#elif CONFIG_KERNEL_FAULT_VERBOSITY >= 2
	serial_print_p(PSTR("\n***** Kernel "));
	switch (reason) {
	case K_FAULT_ASSERT:
		serial_print_p(PSTR("Assert"));
		break;
	case K_FAULT_STACK_SENTINEL:
		serial_print_p(PSTR("Stack Sentinel"));
		break;
	case K_FAULT_STACK_OVERFLOW:
		serial_print_p(PSTR("Stack Overflow"));
		break;
	case K_FAULT_MEMORY:
		serial_print_p(PSTR("Memory"));
		break;
	case K_FAULT_THREAD_TERMINATED:
		serial_print_p(PSTR("Thread Terminated"));
		break;
	case K_FAULT_KERNEL_HALT:
		serial_print_p(PSTR("Halt"));
		break;
	default:
		serial_print_p(PSTR("Fault"));
		break;
	}
	serial_print_p(PSTR(" *****\n"));

	k_thread_dump(z_ker.current);
#endif /* CONFIG_KERNEL_FAULT_VERBOSITY */

	asm("jmp _exit");

	__builtin_unreachable();
}