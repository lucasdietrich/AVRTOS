#include "signal.h"

#include <util/atomic.h>

#include "kernel.h"

/*___________________________________________________________________________*/

void k_signal_init(struct k_signal *sig, uint8_t initial_value)
{
    sig->signal = initial_value;
    sig->flags = K_POLL_STATE_NOT_READY;
    sig->waitqueue = NULL;
}

void k_signal_raise(struct k_signal *sig)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        sig->flags |= K_POLL_STATE_SIGNALED;

        struct ditem * wthread;
        while((wthread = dlist_dequeue(&sig->waitqueue)) != NULL) 
        {
            struct k_thread *const th = THREAD_OF_QITEM(wthread);
            th->wsig.next = NULL;
            _k_wake_up(th);
        }
        k_yield();
    }
}

uint8_t k_poll_signal(struct k_signal *sig, k_timeout_t timeout)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (!TEST_BIT(sig->flags, K_POLL_STATE_SIGNALED) && timeout.value != K_NO_WAIT.value)
        {
            dlist_queue(&sig->waitqueue, &k_current->wsig);
            _k_reschedule(timeout);
            k_yield();
            dlist_remove(&sig->waitqueue, &k_current->wsig);
        }

        return sig->signal;
    }

    __builtin_unreachable();
}

/*___________________________________________________________________________*/