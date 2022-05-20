#include <avr/pgmspace.h>

#include <avrtos/misc/uart.h>
#include <avrtos/kernel.h>

/* Minimum footprint (ATmega2560)
RAM:   [          ]   0.4% (used 31 bytes from 8192 bytes)
Flash: [          ]   0.8% (used 1948 bytes from 253952 bytes)
*/
int main(void)
{
	usart_init();

	for (;;) {
		usart_print_p(PSTR("Hello\n"));
		k_wait(K_SECONDS(1));
	}
}
