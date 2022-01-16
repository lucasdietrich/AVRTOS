#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

char chrs[2] = {'a', 'b'};
void mythread(char *ctx);

K_THREAD_DEFINE(t1, mythread, 0x80, K_PREEMPTIVE, &chrs[0], 'A');
// K_THREAD_DEFINE(t2, mythread, 0x80, K_PREEMPTIVE, &chrs[1], 'B');

int main(void)
{
	led_init();
	usart_init();

	printf_P(PSTR("**************************\n"));
	_delay_ms(1000);
	k_thread_dump_all();

	// USART_DUMP_RAM_ALL();

	sei();

	uint32_t i = 0;


	while (1) {
		// usart_transmit('m');

		// k_wait(K_SECONDS(1));

		// k_yield();

		i++;

		usart_transmit('M');
		
		_delay_ms(1000);

		if (i % 10 == 0) {
			k_thread_dump_all();
		}
	}
}

void mythread(char *ctx)
{
	uint32_t i = 0;

	while (1) {
		i++;

		usart_transmit(*ctx);
		
		_delay_ms(1000);

		if (i % 10 == 0) {
			k_thread_dump_all();
		}
	}
}

/*___________________________________________________________________________*/