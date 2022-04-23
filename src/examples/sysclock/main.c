#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#define PERIOD  1000

void thread_led(void *p);

K_THREAD_DEFINE(ledon, thread_led, 0x100, K_PRIO_DEFAULT, NULL, 'L');

int main(void)
{
	led_init();
	usart_init();

	k_sleep(K_FOREVER);
}

void thread_led(void *p)
{
	while (1) {
		k_thread_dump(_current);

		led_on();
		k_sleep(K_MSEC(PERIOD));
		led_off();
		k_sleep(K_MSEC(PERIOD));
	}
}