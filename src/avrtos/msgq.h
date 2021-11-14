#ifndef _AVRTOS_MSGQ_H_
#define _AVRTOS_MSGQ_H_

#include <avrtos/kernel.h>
#include <avrtos/dstruct/dlist.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct k_msgq
{
        struct ditem waitqueue;

        uint8_t msg_size;
        uint8_t max_msgs;
        uint8_t used_msgs;

        void *buf_start;
        void *buf_end;

        void *read_cursor;
        void *write_cursor;
};

/*___________________________________________________________________________*/

#define K_MSGQ_INIT(msgq, p_buffer, d_msg_size, d_max_msgs) \
{ \
        .waitqueue = DLIST_INIT(msgq.waitqueue), \
        .msg_size = d_msg_size, \
        .max_msgs = d_max_msgs, \
        .used_msgs = 0, \
        .buf_start = p_buffer, \
        .buf_end = p_buffer + ((d_msg_size)*(d_max_msgs)), \
        .read_cursor = p_buffer, \
        .write_cursor = p_buffer, \
}

#define K_MSGQ_DEFINE(msgq, p_buffer, d_msg_size, d_max_msgs) \
        static struct k_msgq msgq = K_MSGQ_INIT(msgq, p_buffer, d_msg_size, d_max_msgs)


/*___________________________________________________________________________*/

K_NOINLINE void k_msgq_init(struct k_msgq *msgq, char *buffer,
                            size_t msg_size, uint32_t max_msgs);

K_NOINLINE int8_t k_msgq_cleanup(struct k_msgq *msgq);

K_NOINLINE int8_t k_msgq_put(struct k_msgq *msgq, const void *data,
                          k_timeout_t timeout);

K_NOINLINE int8_t k_msgq_get(struct k_msgq *msgq, void *data,
                          k_timeout_t timeout);

K_NOINLINE void k_msgq_purge(struct k_msgq *msgq);

K_NOINLINE int8_t k_msgq_peek(struct k_msgq *msgq, void *data);

K_NOINLINE uint8_t k_msgq_num_free_get(struct k_msgq *msgq);

K_NOINLINE uint8_t k_msgq_num_used_get(struct k_msgq *msgq);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif