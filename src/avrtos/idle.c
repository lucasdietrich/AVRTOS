#include "idle.h"

#include <avr/sleep.h>
/*___________________________________________________________________________*/

#if KERNEL_THREAD_IDLE

K_THREAD_DEFINE(_k_idle, _k_idle_entry, K_TRHEAD_IDLE_STACK_SIZE, K_PREEMPTIVE, NULL, 'I');

void _k_idle_entry(void *context)
{
        for (;;) {
                /* only an interrupt can wake up the CPU after this instruction */
                sleep_cpu();
        }
}

extern struct ditem *runqueue;

bool _k_runqueue_idle(void)
{
        return runqueue == &_k_idle.tie.runqueue;
}

#endif

/*___________________________________________________________________________*/