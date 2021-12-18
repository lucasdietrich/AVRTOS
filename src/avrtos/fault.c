#include "fault.h"

#include "misc/uart.h"

#include <avr/pgmspace.h>


static const char reasons[][10] PROGMEM = {
	"ASSERT",
	"SENTINEL",
	"<UNKNOWN>",
};

static const char *reason_to_str(uint8_t reason)
{
	if (reason >= ARRAY_SIZE(reasons)) {
		reason = ARRAY_SIZE(reasons) - 1;
	}

	return reasons[reason];
}


void __fault(uint8_t reason)
{
	static const char assert_msg[] PROGMEM =
		"***** Kernel Fault *****\n Reason > ";

	cli();

	usart_print_p(assert_msg);
	usart_print_p(reason_to_str(reason));

	asm("jmp _exit");

	__builtin_unreachable();
}