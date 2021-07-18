#include "kernel.h"
#include "multithreading.h"

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
    // k_xtqueue_shift(&scheduled_threads, KERNEL_TIME_SLICE);
}

void k_sleep(k_timeout_t timeout)
{
    if (timeout.delay != 0)
    {
        cli();


        k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/