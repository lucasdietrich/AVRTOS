#include "kernel.h"
#include "multithreading.h"

/*___________________________________________________________________________*/

static struct k_xtqueue_item_t *scheduled_threads = NULL;

static DEFINE_DLIST(pending_threads);

/*___________________________________________________________________________*/

void k_yield(void)
{
    // k_reschedule(); // reschedule self thread

    _k_yield(); // arch kernel yield
}

void k_scheduler_init(void)
{
    cli();
    for (uint8_t i = 1; i < ARRAY_SIZE(k_thread.list); i++)
    {
        dlist_queue(&pending_threads, &k_thread.list[i]->pending);
    }
    sei();
}

// unsafe
void k_reschedule(void)
{
    dlist_queue(&pending_threads, &k_thread.current->pending);
}

#include "misc/uart.h"

// todo write this function in assembly
struct thread_t *_k_scheduler(void)
{
    k_reschedule(); // reschedule same thread

    // need to be donne 
    struct k_xtqueue_item_t * item = k_xtqueue_pop(&scheduled_threads);
    if (item != NULL)
    {
        usart_transmit('i');
        k_thread.current = ((struct thread_t*) item->p_context);
    }
    else
    {
        struct ditem* thread_toexec = dlist_dequeue(&pending_threads);
        if (thread_toexec != &pending_threads)
        {
            // usart_transmit('p');
            k_thread.current = CONTAINER_OF(thread_toexec, struct thread_t, pending);
        }
    }

    // k_thread.current->state = RUNNING;
    return k_thread.current;
}

void k_cpu_idle(void)
{
    // TODO
}

/*___________________________________________________________________________*/

#include "misc/uart.h"

void _k_system_shift(void)
{
    k_xtqueue_shift(&scheduled_threads, KERNEL_TIME_SLICE);
}

void k_sleep(k_timeout_t timeout)
{
    if (timeout.delay != 0)
    {
        struct k_xtqueue_item_t item = {
            .timeout = timeout.delay,
            .next = NULL,
            .p_context = k_thread.current,
        };

        cli();

        // k_thread.current->state = WAITING;

        k_xtqueue_schedule(&scheduled_threads, &item);

        k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/