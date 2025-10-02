/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_TQUEUE_H
#define _AVRTOS_TQUEUE_H

#include "../defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Scheduling queue data structure
 *
 * - n : number of items in the list
 *
 * tqueue_schedule/tqueue_pop are O(1)
 * tqueue_remove is O(n)
 * tqueue_shift is O(n)
 *
 * TODO tqueue_remove could be optimized to O(1), using a doubly linked list
 */

struct titem {
    union {
        k_delta_t delay_shift;
        k_delta_t abs_delay;
        k_delta_t timeout;
    };
    struct titem *next;
};

typedef struct titem titem_t;
typedef struct titem tqueue_t;

#define DEFINE_TQUEUE(name) struct titem *name = NULL
#define INIT_TITEM(timeout_ms)                                                           \
    {                                                                                    \
        .timeout = timeout_ms, .next = NULL                                              \
    }
#define INIT_TITEM_DEFAULT() INIT_TITEM(0)

#define DEFINE_TITEM(name) struct titem name = INIT_TITEM_DEFAULT()

/**
 * @brief Schedule an event by adding it to the list.
 * If the new event has the same timeout as another event
 * already in the list, it will be added after i.
 *
 * Assumptions :
 * - root is not null
 * - item is not null
 * - item->next is null
 * - item->timeout is set
 *
 * @param root
 * @param item
 */
void z_tqueue_schedule(struct titem **root, struct titem *item);

/**
 * @see z_tqueue_schedule
 *
 * Assumptions :
 *  - root is not null
 *
 * @param root
 * @param item
 * @param timeout
 */
void tqueue_schedule(struct titem **root, struct titem *item, k_delta_t timeout);

/**
 * @brief Shift the queue time of {time_passed}
 *
 * Assumptions :
 *  - root is not null
 *
 * @param root
 * @param time_passed
 */
void tqueue_shift(struct titem **root, k_delta_t time_passed);

/**
 * @brief Pop an item from the time queue.
 *
 * Note: this function doesn't alter/reset the item->tie nor the item->next members.
 *
 * Assumptions:
 * - root is not null
 *
 * @param root
 * @return struct titem*
 */
struct titem *tqueue_pop(struct titem **root);

/**
 * @brief
 *
 * @param root
 * @return struct titem*
 */
struct titem *tqueue_pop_reschedule(struct titem **root, k_delta_t timeout);

/**
 * @brief Remove an item from the time queue.
 * Item->next attribute is set to null if found.
 *
 * Note: this function is very expensive in term of time if there are a
 *  lot of items in the titem queue.
 *
 * Assumptions :
 *  - root is not null
 *  - item is not null
 *  - item is in root tqueue
 *
 * @param root
 * @param item Item to remove if exists in the time queue.
 */
void tqueue_remove(struct titem **root, struct titem *item);

#ifdef __cplusplus
}
#endif

#endif