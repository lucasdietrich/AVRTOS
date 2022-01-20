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
	case K_THREAD_TERMINATED:
		return PSTR("THREAD_TERMINATED");
	case K_FAULT_INTERRUPT:
		return PSTR("INTERRUPT");
	default:
		return PSTR("<UNKNOWN>");
	}
}


void __fault(uint8_t reason)
{
	cli();

	usart_print_p(PSTR("***** Kernel Fault *****\n Reason > "));
	usart_print_p(reason_to_str(reason));

	asm("call __debug");

	asm("jmp _exit");

	__builtin_unreachable();
}