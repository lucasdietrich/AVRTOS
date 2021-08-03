#include "idle.h"

/*___________________________________________________________________________*/

#if KERNEL_THREAD_IDLE

// few problems:
// - _k_scheduler function is using current thread stack, when all registers are pushed (35) : 
//  minimal stacks may be full already when calling the scheduler
// - solution is to move scheduler before having pushed all the registers
// - using a stack for idle is not a good idea to me -> maybe creating a kernel specific thread ?
// - or use this idle thread to to kernel works (as k_works/workqueue, etc...), call it "system thread/kernel thread"
//

// K_THREAD_MINSTACK_DEFINE(_k_idle, _k_idle_entry, K_PRIO_PREEMPT(K_PRIO_MIN), NULL, NULL, 'K');
K_THREAD_DEFINE(_k_idle, _k_idle_entry, K_THREAD_STACK_MIN_SIZE + 100, K_PRIO_PREEMPT(K_PRIO_MIN), NULL, NULL, 'K');

void _k_idle_entry(void* context)
{
    while(1) {
        // todo : low power
        // warning : this thread has NO stack
    }
}

#endif

/*___________________________________________________________________________*/