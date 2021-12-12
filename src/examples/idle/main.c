#include <stdio.h>
#include <util/delay.h>

#include <avrtos/misc/uart.h>
#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/**
 * @brief Example showing show what happens when IDLE thread is
 * disabled and all thread are sleeping.
 * 
 * Enable option CONFIG_KERNEL_THREAD_IDLE or make sure at least one thread is always
 * ready to solve the issue.
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
		printf("Hello World, counter = %lx !\n", counter++);

		k_sleep(K_MSEC(1000));
	}
}

void thread(void *ctx);

K_THREAD_DEFINE(t1, thread, 0x100, K_PREEMPTIVE, NULL, 'T');

void thread(void *ctx)
{
	_delay_ms(1000.0);

	k_sleep(K_FOREVER);
}