#include <avrtos/kernel.h>
#include <avrtos/debug.h>

int main()
{
	usart_init();
	k_thread_dump_all();

	for (;;) {
		k_sleep(K_SECONDS(1));
		usart_transmit('u');
	}
}