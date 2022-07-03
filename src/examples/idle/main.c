#include <stdio.h>
#include <util/delay.h>

#include <avrtos/misc/uart.h>
#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/**
 * @brief Example showing show what happens when IDLE thread is
 * disabled and all thread are sleeping.
 *
 * Enable option CONFIG_KERNEL_THREAD_IDLE or make sure at least one thread is always running.
 *
 * @return int
 */

int main(void)
{
	static uint32_t counter = 0;
	usart_init();

	irq_enable();

	k_thread_dump_all();

	for (;;) {
		printf_P(PSTR("Hello World, counter = %lx !\n"), counter++);

		k_sleep(K_MSEC(1000));
	}
}