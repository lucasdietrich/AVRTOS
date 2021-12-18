#include <avrtos/kernel.h>
#include <avrtos/misc/uart.h>
#include <avrtos/debug.h>

#define THREADS_COUNT   15
#define MAX_DELAY_MS	5000

#define K_MODULE K_MODULE_APPLICATION

static void thread(void *);
static void event_handler(void *ev);

struct mystruct
{
	struct k_event ev; 
	struct k_signal sig;
	struct k_thread thread;
	char stack[0x50];
};

struct mystruct threads[THREADS_COUNT];

int main(void)
{
	usart_init();

	k_thread_dump_all();
	
	struct mystruct *ms;

	for (ms = threads; ms < threads + ARRAY_SIZE(threads); ms++) {
		k_event_init(&ms->ev, event_handler);
		k_signal_init(&ms->sig);
		k_thread_create(&ms->thread, thread, ms->stack,
				sizeof(ms->stack), K_COOPERATIVE, ms, '0' + (ms - threads));
		k_start(&ms->thread);
	}

	/* act as IDLE thread */
	for (;;) {
		k_idle();
	}
}

static unsigned int random(void)
{
	K_PRNG_DEFINE_DEFAULT(prng);
	
	/* protect with mutex or K_SCHED_LOCK_CONTEXT if threads are preemptive */
	return k_prng_get(&prng);
}

static void event_handler(void *ev)
{
	struct mystruct *ms = CONTAINER_OF(ev, struct mystruct, ev);

	k_signal_raise(&ms->sig, 0);
}

static void thread(void *p)
{
	uint16_t rdm_ms;
	uint32_t counter = 0;
	struct mystruct *ms = (struct mystruct *) p;

	/* protect with K_SCHED_LOCK_CONTEXT if threads are preemptive */
	printf_P(PSTR("Thread %c started\n"), _current->symbol);

	for (;;) {
		rdm_ms = random() % MAX_DELAY_MS;
		k_event_schedule(&ms->ev, K_MSEC(MAX(rdm_ms, 1))); /* make sure timeout is not 0 */

		if (k_poll_signal(&ms->sig, K_FOREVER) == 0) {
			K_SIGNAL_SET_UNREADY(&ms->sig);

			/* protect with K_SCHED_LOCK_CONTEXT if threads are preemptive */
			printf_P(PSTR("%c : signaled after %u ms [counter = %lu]\n"),
				 _current->symbol, rdm_ms, ++counter);
		} else {
			__ASSERT_TRUE(false);
		}
	}
}