#include "kernel.h"

/*___________________________________________________________________________*/

struct k_xtqueue_item_t *_k_system_xtqueue = NULL;

/*___________________________________________________________________________*/

struct thread_t *_k_scheduler(void)
{
    k_thread.current_idx = (k_thread.current_idx + 1) % k_thread.count;

    k_thread.current = k_thread.list[k_thread.current_idx];
    
    return k_thread.current;
}

/*___________________________________________________________________________*/


void _k_system_shift(k_delta_ms_t time_passed)
{
    k_xtqueue_shift(&_k_system_xtqueue, time_passed);
}

void k_sleep(k_timeout_t timeout)
{
    if (timeout.delay != 0)
    {
        cli();

        struct k_xtqueue_item_t item = {
            .timeout = timeout.delay,
            .next = NULL,
            .p_context = k_thread.current,
        };

        k_thread.current->state = WAITING;

        k_xtqueue_schedule(&_k_system_xtqueue, &item);

        k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/