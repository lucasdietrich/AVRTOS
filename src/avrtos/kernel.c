#include "kernel.h"
#include "multithreading.h"

/*___________________________________________________________________________*/

struct k_xtqueue_item_t *_k_system_xtqueue = NULL;

/*___________________________________________________________________________*/


#include "misc/uart.h"

// todo write this function in assembly
struct thread_t *_k_scheduler(void)
{
    // if current thread was still running keeps it's state to ready
    if (k_thread.current->state == RUNNING)
    {
        k_thread.current->state = READY;
    }

    struct k_xtqueue_item_t * item = k_xtqueue_pop(&_k_system_xtqueue);
    if (item == NULL)
    {
        uint8_t actual_idx;
        bool found = false;
        for (uint8_t shift = 0; shift < ARRAY_SIZE(k_thread.list) - 1; shift++)
        {
            actual_idx = (k_thread.current_idx + shift + 1) % ARRAY_SIZE(k_thread.list);
            if (k_thread.list[actual_idx]->state == READY)
            {
                found = true;
                break;
            }
        }
        
        if (found)
        {
            k_thread.current_idx = actual_idx;
            k_thread.current = k_thread.list[actual_idx];
        }
        else // not thread found
        {
            usart_transmit('I');

            k_cpu_idle();
        }
    }
    else
    {
        k_thread.current = ((struct thread_t*) item->p_context);
    }

    k_thread.current->state = RUNNING;
    k_thread.current_idx = &k_thread.current - k_thread.list;   // illegal, but it's the idea
    return k_thread.current;
}

__attribute__((naked)) void k_yield(void)
{
    k_thread.current->state = READY;    // to this in assembler
    
    _k_yield();
}

void k_cpu_idle(void)
{
    // TODO
}

/*___________________________________________________________________________*/

#include "misc/uart.h"

void _k_system_shift(void)
{
    k_xtqueue_shift(&_k_system_xtqueue, KERNEL_TIME_SLICE);
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