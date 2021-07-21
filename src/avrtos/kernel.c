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
    // main thread is the first running (ready or not)
    for (uint8_t i = 1; i < k_thread.count; i++)
    {
        if (k_thread.list[i]->flags.state == READY) // only queue ready threads
        {
            dlist_queue(runqueue, &k_thread.list[i]->tie.runqueue);
        }
    }
}

#include "misc/uart.h"

// todo write this function in assembly
struct thread_t *_k_scheduler(void)
{
    void *next = (struct titem*) tqueue_pop(&events_queue);
    if (next != NULL) // priority to expired timer
    {
        CONTAINER_OF(next, struct thread_t, tie.event)->flags.state = READY;

        push_ref(&runqueue, next);
        usart_transmit('T');
    }
    else if (k_thread.current->flags.state == WAITING)
    {
        // next = (struct ditem *)dlist_dequeue(runqueue);
        // pop_ref(&runqueue);
        
        usart_transmit('W');
    }
    else
    {
        // next = (struct ditem *)ref_requeue_top(&runqueue);
        ref_requeue_top(&runqueue);

        usart_transmit('N');
    }

    return k_thread.current = CONTAINER_OF(runqueue, struct thread_t, tie);
}

#include "avrtos/dstruct/debug.h"

void _thread_symbol(struct ditem * item)
{
    usart_transmit(CONTAINER_OF(item, struct thread_t, tie.runqueue)->symbol);
}

void print_runqueue(void)
{
    print_dlist(runqueue, _thread_symbol);
}

void k_cpu_idle(void)
{
    // TODO
    // loop on low power and regularly check scheduler
}

/*___________________________________________________________________________*/

#include "misc/uart.h"

void _k_system_shift(void)
{
    tqueue_shift(&events_queue, KERNEL_TIME_SLICE);
}

void k_sleep(k_timeout_t timeout)
{
    if (timeout.value != 0)
    {
        cli();

        k_thread.current->flags.state = WAITING;

        pop_ref(&runqueue);

        tqueue_schedule(&events_queue, &k_thread.current->tie.event, timeout.value);
     
        k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/