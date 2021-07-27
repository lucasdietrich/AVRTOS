#include "kernel.h"

/*___________________________________________________________________________*/

struct ditem *runqueue = &_k_thread_main.tie.runqueue; 

struct titem *events_queue = NULL;

/*___________________________________________________________________________*/

void k_queue(struct thread_t *th)
{
    push_back(runqueue, &th->tie.runqueue);
}

void k_yield(void)
{
    _k_yield();
}

extern struct thread_t __k_threads_start;
extern struct thread_t __k_threads_end;

uint8_t _k_thread_count;

void _k_kernel_init(void)
{
    _k_thread_count = &__k_threads_end - &__k_threads_start;

    // main thread is the first running (ready or not)
    for (uint8_t i = 0; i < _k_thread_count; i++)
    {
        if ((&__k_threads_start)[i].state == READY) // only queue ready threads
        {
            dlist_queue(runqueue, &(&__k_threads_start)[i].tie.runqueue);
        }
    }
}



#if !KERNEL_SCHEDULER_DEBUG

struct thread_t *_k_scheduler(void)
{
    void *next = (struct titem*) tqueue_pop(&events_queue);
    if (next != NULL)
    {
        push_ref(&runqueue, next);
    }
    else if(k_current->state != WAITING)
    {
        ref_requeue_top(&runqueue);
    }
    THREAD_OF_TITEM(runqueue)->state = READY;

    return k_current = CONTAINER_OF(runqueue, struct thread_t, tie);
}

#else

#include "misc/uart.h"

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
        push_ref(&runqueue, next);

        usart_transmit('!');
    }
    else if (k_current->state == WAITING)
    {
        usart_transmit('~');
    }
    else
    {
 
        // next = (struct ditem *)ref_requeue_top(&runqueue);
        ref_requeue_top(&runqueue);

        usart_transmit('>');
    }

    _thread_symbol(runqueue);

    THREAD_OF_TITEM(runqueue)->state = READY;

    return k_current = CONTAINER_OF(runqueue, struct thread_t, tie);
}
#endif


void k_cpu_idle(void)
{
    // TODO loop on low power and regularly check scheduler
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

        k_current->state = WAITING;
        
        pop_ref(&runqueue);

        if(timeout.value != K_FOREVER.value)
        {
            tqueue_schedule(&events_queue, &k_current->tie.event, timeout.value);
        }
     
        k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/

