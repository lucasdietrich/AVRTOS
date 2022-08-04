/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

void thread(struct k_prng *prng);

K_PRNG_DEFINE_DEFAULT(p1);
K_PRNG_DEFINE_DEFAULT(p2);
K_PRNG_DEFINE(p3, 0xABCDEF12, 0x12345678);
K_PRNG_DEFINE(p4, 0xABCDEF12, 0x12345678);

K_THREAD_DEFINE(t1, thread, 0x100, K_PREEMPTIVE, &p1, 'A');
K_THREAD_DEFINE(t2, thread, 0x100, K_PREEMPTIVE, &p2, 'B');
K_THREAD_DEFINE(t3, thread, 0x100, K_PREEMPTIVE, &p3, 'C');

/*___________________________________________________________________________*/

uint8_t buffer[256];

int main(void)
{
	usart_init();

	k_thread_dump_all();

	k_prng_get_buffer(&p4, buffer, sizeof(buffer));
	for (uint16_t i = 0; i < sizeof(buffer); i++) {
		usart_hex(buffer[i]);
		usart_transmit(' ');
		if ((i & 0xF) == 0xF)
			usart_transmit('\n');
	}
	usart_transmit('\n');

	k_sleep(K_FOREVER);
}

void thread(struct k_prng *prng)
{
	uint16_t number;

	for (;;) {
		number = k_prng_get(prng);

		usart_transmit(_current->symbol);
		usart_print_p(PSTR(" : "));
		usart_hex16(number);
		usart_transmit('\n');

		k_sleep(K_MSEC(500));
	}
}

/*___________________________________________________________________________*/