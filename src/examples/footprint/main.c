#include <avr/pgmspace.h>

#include <avrtos/misc/uart.h>
#include <avrtos/kernel.h>

#define K_MODULE K_MODULE_APPLICATION

/*
Total RAM : 63 bytes :
 - Main stack is 0x30 = 32
 - _k_thread_main = 18

TODO gather all this in a buffer.
 - Events_queue = 2
 - runqueue = 2
 - _k_timers_runqueue = 2
 - _k_uptime_ms = 4
 - _current = 2
 */

static void sleep(k_timeout_t ms)
{
	uint32_t start = k_uptime_get_ms32();
	uint32_t now;

	do {
		now = k_uptime_get_ms32();
	} while (now - start < ms.value);
}

int main(void)
{
	irq_enable();

	usart_init();

	for (;;) {
		usart_print_p(PSTR("Hello\n"));
		sleep(K_SECONDS(1));
	}
}
