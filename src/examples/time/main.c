#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#define K_MODULE K_MODULE_APPLICATION

void k_show_uptime(void)
{
        struct timespec ts;
        k_timespec_get(&ts);

        uint32_t seconds = ts.tv_sec;
        uint32_t minutes = seconds / 60;
        uint32_t hours = minutes / 60;

        printf_P(PSTR("%02lu:%02hhu:%02hhu [%lu.%03u s] : "),
                 hours, (uint8_t)(minutes % 60), (uint8_t)(seconds % 60),
                 ts.tv_sec, ts.tv_msec);
}

void thread(void *ctx);

K_THREAD_DEFINE(t1, thread, 0x100, K_PREEMPTIVE, NULL, '1');

int main(void)
{
	usart_init();

	k_thread_dump_all();

	irq_enable();

	uint64_t now;
	uint32_t now32;
	uint32_t timestamp;

	for(;;) {
		// now = k_uptime_get_ms64();

		k_uptime_ms64(&now);
		k_uptime_ms32(&now32);
		timestamp = k_time_get();
		
		k_show_uptime();

		printf_P(PSTR(" %lu 64ms - %lu 32ms - "), (uint32_t) now, now32);

		printf_P(PSTR("%lu s - "), k_uptime_get());

		printf_P(PSTR("%lu s (timestamp)\n"), timestamp);

		k_sleep(K_SECONDS(1));
	}
}

void thread(void *ctx)
{
	uint32_t sec = 100000;
	for(;;) {
		k_time_set(sec);

		sec += 100000;

		k_sleep(K_SECONDS(5));
	}
}