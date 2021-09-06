#include "fifo.h"

#include <avrtos/kernel.h>
#include <avrtos/dstruct/queue.h>

void k_fifo_init(struct k_fifo *fifo)
{
    fifo->queue = NULL;
    fifo->waitqueue = NULL;
}

void k_fifo_put(struct k_fifo *fifo, struct qitem * item)
{
    __ASSERT_NOTNULL(fifo, K_MODULE_FIFO);
    __ASSERT_NOTNULL(item, K_MODULE_FIFO);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        queue(&fifo->queue, item);

        struct ditem *waiting_thread = dlist_dequeue(&fifo->waitqueue);
        if (waiting_thread != NULL)
        {
            struct k_thread *th = THREAD_OF_QITEM(waiting_thread);

            /* immediate: the first thread in the queue 
                * must be the first to get the semaphore */
            _k_immediate_wake_up(th);

            k_yield();
        }
    }
}

struct qitem * k_fifo_get(struct k_fifo *fifo, k_timeout_t timeout)
{
    __ASSERT_NOTNULL(fifo, K_MODULE_FIFO);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        struct qitem * item = dequeue(&fifo->queue);
        if ((item == NULL) && (timeout.value != 0))
        {
            dlist_queue(&fifo->waitqueue, &k_current->wfifo);

            _k_reschedule(timeout);

            k_yield();

            // TODO use a dlist in order to remove it without condition
            if (TEST_BIT(k_current->flags, K_FLAG_TIMER_EXPIRED))
            {
                dlist_remove(&fifo->waitqueue, &k_current->wfifo);
            }
            
            item = dequeue(&fifo->queue);
        }
        return item;
    }

    __builtin_unreachable();
}

bool k_fifo_is_empty(struct k_fifo *fifo)
{
    return k_fifo_peek_head(fifo) == NULL;
}

struct qitem * k_fifo_peek_head(struct k_fifo *fifo)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        return fifo->queue;
    }

    __builtin_unreachable();
}

struct qitem * k_fifo_peek_tail(struct k_fifo *fifo)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        struct qitem * item = fifo->queue;
        if (item != NULL)
        {
            while(item->next != NULL)
            {
                item = item->next;
            }
        }
        return item;
    }

    __builtin_unreachable();
}