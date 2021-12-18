#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <util/delay.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

struct k_thread *volatile s_thread = NULL;

void thread_entry(void *_c);
void thread_canaries_entry(void *_c);

#if THREAD_ALLOW_RETURN == 0
#error This example requires THREAD_ALLOW_RETURN configuration option to be set
#endif

K_THREAD_DEFINE(thread, thread_entry, 0x100,
		K_PREEMPTIVE, &s_thread, 'T');

K_THREAD_DEFINE(thread_canaries, thread_canaries_entry, 0x100,
		K_PREEMPTIVE, &s_thread, 'C');

K_SEM_DEFINE(mysem, 0, 1);

int main(void)
{
	usart_init();

	k_thread_dump_all();

	irq_enable();

	for (;;) {
		k_sem_take(&mysem, K_FOREVER);

		usart_print("Thread terminated : ");
		usart_hex16(s_thread->symbol);
		usart_transmit('\n');

		/* rebuild stack */
		k_thread_create(s_thread, thread_entry,
				K_STACK_START(s_thread->stack.end,
					      s_thread->stack.size),
				s_thread->stack.size,
				K_PREEMPTIVE, NULL, 'T');
		k_start(s_thread);

		usart_printl("Thread started again");
		k_start(s_thread);
	}

	k_sleep(K_FOREVER);
}

void thread_entry(void *_c)
{
	s_thread = _current;

	for (uint_fast8_t i = 0; i < 5; i++) {
		usart_printl("Hello !");
		k_sleep(K_MSEC(250));
	}

	/* make sure the cpu is not released
	 * until total termination of the thread */
	k_sched_lock();
	k_sem_give(&mysem);
}

void thread_canaries_entry(void *_c)
{
	for (;;) {
		k_thread_dump_all();
		dump_stack_canaries();

		k_sleep(K_SECONDS(30));
	}
}