#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

void timer_handler(struct k_timer *timer)
{
	printf("uptime: %lu seconds\n", k_uptime_get());
}

K_TIMER_DEFINE(mstimer, timer_handler, K_MSEC(1000), 1000);

int main(void)
{
	led_init();
	usart_init();

	k_thread_dump_all();

	irq_enable();

	uint8_t state = 0;

	for(;;) {

		led_set(state);
		state = 1 - state;

		k_sleep(K_MSEC(5000));

		dump_stack_canaries();
	}
}

void thread(void *ctx);

K_THREAD_DEFINE(ta, thread, 0x200, K_PREEMPTIVE, NULL, 'A');

void thread(void *ctx)
{
	uint64_t last = 0L;
	uint64_t now = 0L;

	const uint32_t period = 1000LLU;

	for (;;) {
		now = k_uptime_get_ms64();
		if (now - last >= period) {
			last = now;
			printf("now = %lu %lu\n", (uint32_t) (now >> 32), (uint32_t) now);
		}
	}
}