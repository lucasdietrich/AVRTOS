#include "semaphore.h"

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#include <util/atomic.h>

extern struct ditem* runqueue;
extern struct titem* events_queue;

void k_sem_init(struct k_sem* sem, uint8_t initial_count, uint8_t limit)
{
    sem->limit = limit;
    sem->count = MIN(limit, initial_count);
    dlist_init(&sem->waitqueue);
}

uint8_t k_sem_take(struct k_sem* sem, k_timeout_t timeout)
{
    int8_t get;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        get = _k_sem_take(sem);
        if (get != 0) {
            if (0 == _k_pend_current(&sem->waitqueue, timeout)) {
                get = _k_sem_take(sem);
            }
        }
    }

    if (get == 0) {
        __K_DBG_SEM_TAKE(_current);
    }

    return get;
}

void k_sem_give(struct k_sem* sem)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        __K_DBG_SEM_GIVE(_current);

        _k_sem_give(sem);

        _k_unpend_first_thread(&sem->waitqueue);
    }
}