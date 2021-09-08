#include "fifo.h"

#include <avrtos/kernel.h>
#include <avrtos/dstruct/queue.h>

void k_fifo_init(struct k_fifo* fifo)
{
    fifo->queue = NULL;
    dlist_init(&fifo->waitqueue);
}

void k_fifo_put(struct k_fifo* fifo, struct qitem* item)
{
    __ASSERT_NOTNULL(fifo, K_MODULE_FIFO);
    __ASSERT_NOTNULL(item, K_MODULE_FIFO);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        queue(&fifo->queue, item);

        _k_wakeup_notify_object(&fifo->waitqueue);
    }
}

struct qitem* k_fifo_get(struct k_fifo* fifo, k_timeout_t timeout)
{
    __ASSERT_NOTNULL(fifo, K_MODULE_FIFO);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        struct qitem* item = dequeue(&fifo->queue);
        if (item == NULL) {
            if (0 == _k_waiting_object(&fifo->waitqueue, timeout)) {
                item = dequeue(&fifo->queue);
            }
        }
        return item;
    }

    __builtin_unreachable();
}

bool k_fifo_is_empty(struct k_fifo* fifo)
{
    return k_fifo_peek_head(fifo) == NULL;
}

struct qitem* k_fifo_peek_head(struct k_fifo* fifo)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        return fifo->queue;
    }

    __builtin_unreachable();
}

struct qitem* k_fifo_peek_tail(struct k_fifo* fifo)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        struct qitem* item = fifo->queue;
        if (item != NULL) {
            while (item->next != NULL) {
                item = item->next;
            }
        }
        return item;
    }

    __builtin_unreachable();
}