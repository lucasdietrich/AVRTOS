#include "mutex.h"

void mutex_define(mutex_t *mutex)
{
    mutex->lock = 0xFFu;
}

uint8_t mutex_lock(mutex_t *mutex)
{
    return _mutex_lock(mutex);
}

uint8_t mutex_lock_wait(mutex_t *mutex, k_timeout_t timeout)
{
    /*
    struct k_scheduled_item_t item = {
        .abs_delay = timeout.delay,
        .next = NULL,
        .p = k_thread_current(),
    };

    _k_schedule_submit(&item);

    // TODO how to notify that the thread is waiting on the mutex
    #warning TODO set curent thread as pending on event

    k_yield();

    return _mutex_lock(mutex);
    */
}