
#include <avrtos/misc/uart.h>
#include <avrtos/kernel.h>
#include <avrtos/debug.h>

void thread(void *p);

// ISR(USART_RX_vect) {
// 	char c = UDR0;
// 	usart_transmit('n');
// }


K_THREAD_DEFINE(t1, thread, K_THREAD_STACK_MIN_SIZE + 10, K_PREEMPTIVE, NULL, '1');

int main(void)
{
	usart_init();

	// UCSR0B = 1 << RXCIE0;

	k_thread_dump_all();

	for (;;) {
		bool success = k_verify_stack_sentinel(&t1);

		printf_P(PSTR("Sentinel status = %d\n"), success ? 1 : 0);

		dump_stack_canaries();

		k_sleep(K_SECONDS(1));
	}
}

void thread(void *p)
{
	uint8_t buffer[9] = "Hello";

	for(;;) {
		for (int i = 0; i < ARRAY_SIZE(buffer); i++) {
			usart_transmit(buffer[i]);
		}

		k_sleep(K_SECONDS(1));
	}
}