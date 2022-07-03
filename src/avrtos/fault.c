#include "fault.h"

#include "misc/uart.h"

#include <avr/pgmspace.h>

static const char *reason_to_str(uint8_t reason)
{
	/*
	 * Note: If using switch case instead if if-else,
	 * need RAM to save the switch data
	 * check for CSWTCH. in .elf symbols
	 */
	if (reason == K_FAULT_ASSERT) {
		return PSTR("ASSERT");
	} else if (reason == K_FAULT_SENTINEL) {
		return PSTR("SENTINEL");
	} else if (reason == K_THREAD_TERMINATED) {
		return PSTR("THREAD_TERMINATED");
	} else if (reason == K_FAULT_INTERRUPT) {
		return PSTR("INTERRUPT");
	} else {
		return PSTR("<UNKNOWN>");
	}
}


void __fault(uint8_t reason)
{
	cli();

	usart_print_p(PSTR("***** Kernel Fault *****\n Reason > "));
	usart_print_p(reason_to_str(reason));
	usart_print_p(PSTR("\n\tth: 0x"));
	usart_hex16((const uint16_t) _current);

	asm("call __debug");

	asm("jmp _exit");

	__builtin_unreachable();
}