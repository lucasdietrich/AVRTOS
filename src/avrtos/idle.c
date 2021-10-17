#include "idle.h"

#include <avr/sleep.h>
/*___________________________________________________________________________*/

#if KERNEL_THREAD_IDLE

// few problems:
// - _k_scheduler function is using current thread stack, when all registers are pushed (35-36) : 
//  minimal stacks may be full already when calling the scheduler
// - using a stack for idle is not a good idea to me
// - use this idle thread to do kernel works, like IDK ??
//

K_THREAD_DEFINE(_k_idle, _k_idle_entry, K_TRHEAD_IDLE_STACK_SIZE, K_PREEMPTIVE, NULL, 'I');

void _k_idle_entry(void* context)
{
    while(1) {
        // warning : this thread has MINIMAL stack

        // only an interrupt can wake up the CPU after this instruction
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