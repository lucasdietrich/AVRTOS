#include "kernel.h"
#include "multithreading.h"

/*___________________________________________________________________________*/

static struct ditem *runqueue = &k_thread_main.tie.runqueue; 

static struct titem *events_queue;

void k_yield(void)
{
    _k_yield(); // arch kernel yield
}

void _k_scheduler_init(void)
{
    for (uint8_t i = 1; i < k_thread.count; i++)
    {
        dlist_queue(runqueue, &k_thread.list[i]->tie.runqueue);
    }
}

#include "misc/uart.h"

// todo write this function in assembly
struct thread_t *_k_scheduler(void)
{
    return k_thread.current = CONTAINER_OF(ref_requeue_top(&runqueue), struct thread_t, tie.runqueue);
}

#include "avrtos/dstruct/debug.h"

void _thread_symbol(struct ditem * item)
{
    struct thread_t *thread = CONTAINER_OF(item, struct thread_t, tie.runqueue);
    usart_transmit(thread->symbol[0]);
    usart_transmit(thread->symbol[1]);
}

void print_runqueue(void)
{
    print_dlist(runqueue, _thread_symbol);
}

void k_cpu_idle(void)
{
    // TODO
}

/*___________________________________________________________________________*/

#include "misc/uart.h"

void _k_system_shift(void)
{
    tqueue_shift(&events_queue, KERNEL_TIME_SLICE);
}

void k_sleep(k_timeout_t timeout)
{
    if (timeout.delay != 0)
    {
        cli();

        k_thread.current->flags.state = WAITING;
     
        k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/