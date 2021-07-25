#include "kernel.h"

/*___________________________________________________________________________*/

static struct ditem *runqueue = &k_thread_main.tie.runqueue; 

static struct titem *events_queue = NULL;

void k_queue(struct thread_t *th)
{
    push_back(runqueue, &th->tie.runqueue);
}

void k_yield(void)
{
    _k_yield();
}

void _k_scheduler_init(void)
{
    // main thread is the first running (ready or not)
    for (uint8_t i = 1; i < k_thread.count; i++)
    {
        if (k_thread.list[i]->state == READY) // only queue ready threads
        {
            dlist_queue(runqueue, &k_thread.list[i]->tie.runqueue);
        }
    }
}

#include "misc/uart.h"

#define KERNEL_SCHEDULER_DEBUG      0

// todo write this function in assembly
// ! mean tqueue item
// ~ thread set to waiting
// > default next thread
struct thread_t *_k_scheduler(void)
{
    // print_tqueue(events_queue, _thread_symbol);
    void *next = (struct titem*) tqueue_pop(&events_queue);
    if (next != NULL)
    {
        CONTAINER_OF(next, struct thread_t, tie.event)->state = READY;

        push_ref(&runqueue, next);

#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('!');
#endif

    }
    else if (k_thread.current->state == WAITING)
    {

#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('~');
#endif
    }
    else
    {
 
        // next = (struct ditem *)ref_requeue_top(&runqueue);
        ref_requeue_top(&runqueue);

#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('>');
#endif
    }

#if KERNEL_SCHEDULER_DEBUG
    _thread_symbol(runqueue);
#endif

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

// todo k_sleep(FOREVER) -> remov thread from queue only
void k_sleep(k_timeout_t timeout)
{
    if (timeout.value != K_NO_WAIT.value)
    {        
        cli();

        k_thread.current->state = WAITING;
        
        pop_ref(&runqueue);

        if(timeout.value != K_FOREVER.value)
        {
            tqueue_schedule(&events_queue, &k_thread.current->tie.event, timeout.value);
        }
     
        k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/