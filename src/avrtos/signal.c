#include "signal.h"

#include <util/atomic.h>

#include <avrtos/kernel.h>
#include <avrtos/kernel_internals.h>


#define K_MODULE K_MODULE_SIGNAL

void k_signal_init(struct k_signal *sig)
{
        __ASSERT_NOTNULL(sig);

        sig->signal = 0u;
        sig->flags = K_POLL_STATE_NOT_READY;
        dlist_init(&sig->waitqueue);
}

void k_signal_raise(struct k_signal *sig, uint8_t value)
{
        __ASSERT_NOTNULL(sig);

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                sig->signal = value;
                sig->flags |= K_POLL_STATE_SIGNALED;

                /* TODO: unpend all threads */
                _k_unpend_first_thread(&sig->waitqueue);
        }
}

int8_t k_poll_signal(struct k_signal *sig, k_timeout_t timeout)
{
        __ASSERT_NOTNULL(sig);

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                if (TEST_BIT(sig->flags, K_POLL_STATE_SIGNALED)) {
                        return 0;
                } else if (!K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
                        return _k_pend_current(&sig->waitqueue, timeout);
                } else {
                        return -1;
                }
        }

        __builtin_unreachable();
}

uint8_t k_poll_cancel_wait(struct k_signal *sig)
{
        __ASSERT_NOTNULL(sig);

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                return _k_cancel_pending(&sig->waitqueue);
        }

        __builtin_unreachable();
}

/*___________________________________________________________________________*/