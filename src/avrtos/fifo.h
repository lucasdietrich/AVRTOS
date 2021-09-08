#ifndef _AVRTOS_FIFO_H
#define _AVRTOS_FIFO_H

#include <avrtos/multithreading.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct k_fifo
{
    struct qitem *queue;         // fifo reference to head item
    struct ditem waitqueue;    // waitqueue of thread waiting to get an item
};

/*___________________________________________________________________________*/

#define K_FIFO_INIT(fifo)                       \
    {                                           \
        .queue = NULL,                          \
        .waitqueue = DLIST_INIT(fifo.waitqueue) \
    }

#define K_FIFO_DEFINE(fifo_name) \
    static struct k_fifo fifo_name = K_FIFO_INIT(fifo_name)

/*___________________________________________________________________________*/

K_NOINLINE void k_fifo_init(struct k_fifo *fifo);

K_NOINLINE void k_fifo_put(struct k_fifo *fifo, struct qitem * item_tie);

K_NOINLINE struct qitem * k_fifo_get(struct k_fifo *fifo, k_timeout_t timeout);

K_NOINLINE bool k_fifo_is_empty(struct k_fifo *fifo);

K_NOINLINE struct qitem * k_fifo_peek_head(struct k_fifo *fifo);

K_NOINLINE struct qitem * k_fifo_peek_tail(struct k_fifo *fifo);

/* concept */
// cancel all threads waiting on the fifo
K_NOINLINE void k_fifo_cancel_wait(struct k_fifo *fifo);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif