#include "fifo.h"

#include <avrtos/kernel.h>
#include <avrtos/dstruct/oqueue.h>

#define K_MODULE    K_MODULE_FIFO

void k_fifo_init(struct k_fifo* fifo)
{
    oqref_init(&fifo->queue);
    dlist_init(&fifo->waitqueue);
}

void k_fifo_put(struct k_fifo* fifo, struct qitem* item)
{
    __ASSERT_NOTNULL(fifo);
    __ASSERT_NOTNULL(item);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        /* If there is a thread waiting on a fifo item, 
         * we to give the item directly to the thread
         * (using thread->swap_data)
         */
        if (_k_unpend_first_thread(&fifo->waitqueue, (void*) item) != 0) {
            /* otherwise we queue the item to the fifo */
            oqueue(&fifo->queue, item);
        }
    }
}

struct qitem* k_fifo_get(struct k_fifo* fifo, k_timeout_t timeout)
{
    __ASSERT_NOTNULL(fifo);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        struct qitem* item = odequeue(&fifo->queue);
        if (item == NULL) {
            if (_k_pend_current(&fifo->waitqueue, timeout) == 0) {
                item = (struct qitem*) _current->swap_data;
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
        return opeek_head(&fifo->queue);
    }

    __builtin_unreachable();
}

struct qitem* k_fifo_peek_tail(struct k_fifo* fifo)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        return opeek_tail(&fifo->queue);
    }

    __builtin_unreachable();
}