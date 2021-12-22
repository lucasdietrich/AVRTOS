#include "fault.h"

#include "misc/uart.h"

#include <avr/pgmspace.h>

static const char *reason_to_str(uint8_t reason)
{
	switch (reason) {
	case K_FAULT_ASSERT:
		return PSTR("ASSERT");
	case K_FAULT_SENTINEL:
		return PSTR("SENTINEL");
	default:
		return PSTR("<UNKNOWN>");
	}
}


void __fault(uint8_t reason)
{
	cli();

	usart_print_p(PSTR("***** Kernel Fault *****\n Reason > "));
	usart_print_p(reason_to_str(reason));

	asm("jmp _exit");

	__builtin_unreachable();
}