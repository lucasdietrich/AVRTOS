/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "event2.h"

#include <util/atomic.h>

#include "assert.h"
#include "defines.h"
#include "errno.h"
#include "kernel.h"
#include "kernel_private.h"
#include "signal.h"
#include "tqueue.h"

#define K_MODULE K_MODULE_EVENT

#if CONFIG_KERNEL_EVENTS2


int8_t k_event2_init(struct k_event2 *event)
{
    if (!z_user(event))
        return -EINVAL;

    if (event->scheduled)
        return -EBUSY;

    event->scheduled = 0;

    return 0;
}

/**
 * @brief Internal function to schedule an event.
 *
 * This function schedules an event to be triggered after a specified timeout.
 * It requires interrupts to be disabled.
 *
 * @param event Pointer to the event structure.
 * @param timeout Timeout in ticks after which the event will be triggered.
 */
void z_event2_schedule(struct k_event_q *evq, struct k_event2 *event, k_ticks_t timeout)
{
    // __ASSERT_TRUE(!event->scheduled);

    // event->scheduled   = 1u;
    event->tie.next    = NULL;
    event->tie.timeout = timeout;

    z_tqueue_schedule(&evq->first, &event->tie);
}

/**
 * @brief Internal function to reschedule an event.
 *
 * This function reschedules an already scheduled event with a new timeout.
 * It requires interrupts to be disabled.
 *
 *
 * @param event Pointer to the event structure.
 * @param timeout New timeout in ticks after which the event will be triggered.
 */
void z_event2_reschedule(struct k_event_q *evq, struct k_event2 *event, k_ticks_t timeout)
{
    // __ASSERT_TRUE(event->scheduled);
    
    tqueue_reschedule(&evq->first, &event->tie, timeout);
}

int8_t k_event2_schedule(struct k_event_q *evq, struct k_event2 *event, k_ticks_t timeout)
{
    if (!z_user(event))
        return -EINVAL;

#if CONFIG_KERNEL_EVENTS2_SYS_EVQ
    if (evq == K_KERNEL_EVQ)
        evq = &z_ker.sys_evq;
#else
    if (!z_user(evq))
        return -EINVAL;
#endif

    int8_t ret         = 0;
    const uint8_t lock = irq_lock();

    if (event->scheduled) {
        ret = -EAGAIN;
        goto exit;
    }

    z_event2_schedule(evq, event, timeout);

exit:
    irq_unlock(lock);
    return ret;
}

int8_t k_event2_cancel(struct k_event_q *evq, struct k_event2 *event)
{
    if (!z_user(event))
        return -EINVAL;

#if CONFIG_KERNEL_EVENTS2_SYS_EVQ
    if (evq == K_KERNEL_EVQ)
        evq = &z_ker.sys_evq;
#else
    if (!z_user(evq))
        return -EINVAL;
#endif

    int8_t ret         = 0;
    const uint8_t lock = irq_lock();

    // if (!event->scheduled) {
    //     ret = -EAGAIN;
    //     goto exit;
    // }

    tqueue_remove(&evq->first, &event->tie);
    // event->scheduled = 0;

exit:
    irq_unlock(lock);
    return ret;
}

bool k_event2_pending(struct k_event2 *event)
{
    if (!z_user(event))
        return false;

    return event->tie.next != NULL;
}

int8_t z_event2_q_advance(struct k_event_q *evq, k_ticks_t delta)
{
    // TODO keep assert
    __ASSERT_NOINTERRUPT();
    __ASSERT_NOTNULL(evq);

    tqueue_shift(&evq->first, delta);

    struct titem *ready = tqueue_peek_head_if_ready(&evq->first);
    if (ready == NULL)
        return 0;
    
    struct k_thread *thread = z_unpend_first_and_swap(&evq->waitqueue, (void *)ready);
    if (thread) {
        /* finally remove the ready event from the queue */
        (void)tqueue_pop_unchecked(&evq->first);
        return 1;
    }

    return 0;
}

int8_t k_event2_q_advance(struct k_event_q *evq, k_ticks_t timeout)
{
    if (!z_user(evq))
        return -EINVAL;

    /* user cannot advance kernel evq */
    if (evq == K_KERNEL_EVQ)
        return -EINVAL;

    int8_t ret = 0;
    const uint8_t lock = irq_lock();

    /* Shift the event queue forward */
    ret = z_event2_q_advance(evq, timeout);

    irq_unlock(lock);
    return ret;
}

int8_t k_event2_pend(struct k_event_q *evq, k_timeout_t timeout, struct k_event2 **evp)
{
    if (!z_user(evp))
        return -EINVAL;

#if CONFIG_KERNEL_EVENTS2_SYS_EVQ
    if (evq == K_KERNEL_EVQ)
        evq = &z_ker.sys_evq;
#else
    if (!z_user(evq))
        return -EINVAL;
#endif

    int8_t ret;
    const uint8_t lock = irq_lock();

    struct titem *ready = tqueue_pop(&evq->first);
    if (ready) {
        *evp = CONTAINER_OF(ready, struct k_event2, tie);
        ret  = 0;
    } else {
        ret = z_pend_current_on(&evq->waitqueue, timeout);
        if (ret == 0) {
            *evp = (struct k_event2 *)z_ker.current->swap_data;
        }
    }

    irq_unlock(lock);
    return ret;
}

#endif /* CONFIG_KERNEL_EVENTS2 */
