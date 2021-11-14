#include "msgq.h"

void k_msgq_init(struct k_msgq *msgq, char *buffer,
                 size_t msg_size, uint32_t max_msgs)
{
        dlist_init(&msgq->waitqueue);
        msgq->max_msgs = max_msgs;
        msgq->msg_size = msg_size;
        msgq->used_msgs = 0;

        msgq->buf_start = buffer;
        msgq->buf_end = buffer + ((msg_size) * (max_msgs));

        msgq->read_cursor = buffer;
        msgq->write_cursor = buffer;
}

int8_t k_msgq_cleanup(struct k_msgq *msgq);

static int8_t _k_msgq_put(struct k_msgq *msgq, const void *data,
                          k_timeout_t timeout)
{

        if (msgq->used_msgs < msgq->max_msgs) {
                struct k_thread *pending_thread =
                        _k_unpend_first_thread(&msgq->waitqueue, NULL);
                if (pending_thread != NULL) {
                        /* a thread is waiting to get a msg, we write directly the
                         * data to the thread buffer. Passed through swap_data
                         */
                        memcpy(pending_thread->swap_data, data, msgq->msg_size);
                } else {
                        /* no thread waiting on a msg, appending the msg to
                         * the msgq buffer */
                        memcpy(msgq->write_cursor, data, msgq->msg_size);
                        msgq->write_cursor = msgq->write_cursor + msgq->msg_size;
                        if (msgq->write_cursor == msgq->buf_end) {
                                msgq->write_cursor = msgq->buf_start;
                        }
                        msgq->used_msgs++;
                }
                return 0;
        } else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
                return -ENOMEM;
        } else {
                /* tells from were the data should be copied
                * where there will be space in the msgq
                 */
                _current->swap_data = (void*) data;

                return _k_pend_current(&msgq->waitqueue, timeout);
        }
}

int8_t k_msgq_put(struct k_msgq *msgq, const void *data,
                  k_timeout_t timeout)
{
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                return _k_msgq_put(msgq, data, timeout);
        }

        __builtin_unreachable();
}

static int8_t _k_msgq_get(struct k_msgq *msgq, void *data,
                          k_timeout_t timeout)
{
        if (msgq->used_msgs > 0) {

                /* copy first message from the msgq to the thread */
                memcpy(data, msgq->read_cursor, msgq->msg_size);
                msgq->read_cursor = msgq->read_cursor + msgq->msg_size;
                if (msgq->read_cursor == msgq->buf_end) {
                        msgq->read_cursor = msgq->buf_start;
                }
                msgq->used_msgs--;

                struct k_thread *pending_thread =
                        _k_unpend_first_thread(&msgq->waitqueue, NULL);
                if (pending_thread != NULL) {
                        /* a thread is waiting to write a msg,
                         * we copy the data from the thread to the msgq
                         */
                        memcpy(msgq->write_cursor, pending_thread->swap_data, msgq->msg_size);
                        msgq->write_cursor = msgq->write_cursor + msgq->msg_size;
                        if (msgq->write_cursor == msgq->buf_end) {
                                msgq->write_cursor = msgq->buf_start;
                        }
                        msgq->used_msgs++;
                }
                return 0;
        } else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
                return -ENOMEM;
        } else {
                /* tells were the data should be copied
                 * where there will be a new message in the msgq
                 */
                _current->swap_data = data;

                return _k_pend_current(&msgq->waitqueue, timeout);
        }
}

int8_t k_msgq_get(struct k_msgq *msgq, void *data,
                  k_timeout_t timeout)
{
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
                return _k_msgq_get(msgq, data, timeout);
        }

        __builtin_unreachable();
}

static void _k_msgq_purge(struct k_msgq *msgq)
{
        /* we wake up any thread that are blocked for writing */

        struct k_thread *pending_thread;
        while ((pending_thread =
                _k_unpend_first_thread(&msgq->waitqueue, NULL)) != NULL) {
                        /* TODO find a way to return an error
                         * to a thread when unpending */
        }

        /* no matter when the read/write cursor are */
        msgq->write_cursor = msgq->read_cursor;
        msgq->used_msgs = 0;
}

void k_msgq_purge(struct k_msgq *msgq)
{
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                _k_msgq_purge(msgq);
        }
}

int8_t k_msgq_peek(struct k_msgq *msgq, void *data)
{
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                if (msgq->used_msgs > 0) {
                        memcpy(data, msgq->read_cursor, msgq->msg_size);

                        return 0;
                } else {
                        return -ENOMSG;
                }
        }

        __builtin_unreachable();
}

uint8_t k_msgq_num_free_get(struct k_msgq *msgq)
{
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                return msgq->max_msgs - msgq->used_msgs;
        }

        __builtin_unreachable();
}
uint8_t k_msgq_num_used_get(struct k_msgq *msgq)
{
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                return msgq->used_msgs;
        }

        __builtin_unreachable();
}
