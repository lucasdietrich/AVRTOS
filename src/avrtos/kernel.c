#include "kernel.h"
#include "multithreading.h"

/*___________________________________________________________________________*/



#include "misc/uart.h"

// todo write this function in assembly
struct thread_t *_k_scheduler(void)
{
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