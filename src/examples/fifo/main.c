/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

k_timeout_t timeouts[2] = {
  K_FOREVER, // K_MSEC(2000)
  K_FOREVER, // K_MSEC(1500)
};

void consumer_thread(k_timeout_t *p_timeout);

K_FIFO_DEFINE(fifo);
K_THREAD_DEFINE(consumer1, consumer_thread, 0x50, K_PREEMPTIVE, &timeouts[0], 'A');
K_THREAD_DEFINE(consumer2, consumer_thread, 0x50, K_PREEMPTIVE, &timeouts[1], 'B');

/*___________________________________________________________________________*/

struct item
{
	char chr;
	struct qitem tie;
};

struct item letters[] = {
  {'1'}, {'2'}, {'3'}, {'4'}, {'5'}
};

/*___________________________________________________________________________*/

int main(void)
{
	led_init();
	usart_init();

	k_thread_dump_all();

	irq_enable();

	uint8_t pos = 0u;
	while (1) {
		usart_transmit(_current->symbol);
		usart_print_p(PSTR(" : Send = "));
		usart_transmit(letters[pos].chr);
		usart_transmit('\n');

		k_fifo_put(&fifo, &letters[pos].tie);

		pos = (pos + 1) % ARRAY_SIZE(letters);

		k_sleep(K_SECONDS(3));
	}
}

void consumer_thread(k_timeout_t *p_timeout)
{
	while (1) {
		struct qitem *tie = k_fifo_get(&fifo, *p_timeout);
		usart_transmit(_current->symbol);
		if (tie != NULL) {
			usart_print_p(PSTR(" : Received = "));
			usart_transmit(CONTAINER_OF(tie, struct item, tie)->chr);
			usart_transmit('\n');
		} else {
			usart_printl_p(PSTR(" : Failed to get a fifo item"));
		}
		k_yield();
	}
}

/*___________________________________________________________________________*/