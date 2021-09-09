#include "mutex.h"

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#include <util/atomic.h>

void k_mutex_init(struct k_mutex* mutex)
{
    mutex->lock = 0xFFu;
    dlist_init(&mutex->waitqueue);
}

uint8_t k_mutex_lock(struct k_mutex* mutex, k_timeout_t timeout)
{
    uint8_t lock;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        lock = _k_mutex_lock(mutex);
        if (lock != 0) {
            if (0 == _k_pend_current(&mutex->waitqueue, timeout)) {
                lock = _k_mutex_lock(mutex);
            }
        }
    }

    if (lock == 0) {
        __K_DBG_MUTEX_LOCKED(_current);    // }
    }

    return lock;
}

void k_mutex_unlock(struct k_mutex* mutex)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        __K_DBG_MUTEX_UNLOCKED(_current); // {

        _k_mutex_unlock(mutex);

        _k_unpend_first_thread(&mutex->waitqueue);
    }
}