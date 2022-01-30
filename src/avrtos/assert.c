#include "assert.h"

#include <avr/pgmspace.h>

#include "misc/uart.h"

static const char *const module_to_str(uint8_t module)
{
	switch (module) {
	case K_MODULE_KERNEL:
		return PSTR("KERNEL");
	case K_MODULE_ARCH:
		return PSTR("ARCH");
	case K_MODULE_SYSCLOCK:
		return PSTR("SYSCLOCK");
	case K_MODULE_THREAD:
		return PSTR("THREAD");
	case K_MODULE_IDLE:
		return PSTR("IDLE");
	case K_MODULE_MUTEX:
		return PSTR("MUTEX");
	case K_MODULE_SEMAPHORE:
		return PSTR("SEMAPHORE");
	case K_MODULE_SIGNAL:
		return PSTR("SIGNAL");
	case K_MODULE_WORKQUEUE:
		return PSTR("WORKQUEUE");
	case K_MODULE_FIFO:
		return PSTR("FIFO");
	case K_MODULE_MEMSLAB:
		return PSTR("MEMSLAB");
	case K_MODULE_TIMER:
		return PSTR("TIMER");
	case K_MODULE_MSGQ:
		return PSTR("MSGQ");
	case K_MODULE_EVENT:
		return PSTR("EVENT");
	case K_MODULE_APPLICATION:
		return PSTR("APPLICATION");
	default:
		return PSTR("UNSPECIFIED");
	}
}

static const char *const acode_to_str(uint8_t acode)
{
	switch (acode) {
	case K_ASSERT_INTERRUPT:
		return PSTR("INTERRUPT");

	case K_ASSERT_NOINTERRUPT:
		return PSTR("NOINTERRUPT");

	case K_ASSERT_LEASTONE_RUNNING:
		return PSTR("LEASTONE_RUNNING");

	case K_ASSERT_THREAD_STATE:
		return PSTR("THREAD_STATE");

	case K_ASSERT_TRUE:
		return PSTR("TRUE");

	case K_ASSERT_FALSE:
		return PSTR("FALSE");

	case K_ASSSERT_NOTNULL:
		return PSTR("NOTNULL");

	case K_ASSSERT_NULL:
		return PSTR("NULL");

	case K_ASSERT_PREEMPTIVE:
		return PSTR("PREEMPTIVE");
	
	case K_ASSERT_COOPERATIVE:
		return PSTR("COOPERATIVE");
	
	case K_ASSERT_SCHED_LOCKED:
		return PSTR("SCHED_LOCKED");

	case K_ASSERT_UNDEFINED:
	default:
		return PSTR("UNDEFINED");
	}
}

void __assert(uint8_t expression, uint8_t module, uint8_t acode, uint16_t line)
{
	if (expression == 0) {
		cli();

		usart_print_p(PSTR("***** Kernel Assertion failed *****\n"
				   "  K_MODULE_"));

		usart_print_p(module_to_str(module));
		usart_transmit(':');
		usart_hex16(line);
		usart_print_p(PSTR(" L [K_ASSERT_"));
		usart_print_p(acode_to_str(acode));
		usart_transmit(']');

		asm("jmp _exit");

		__builtin_unreachable();
	}
}