/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tqueue.h"

// A should be processed in 10 ms
// B should be processed in 10 + 30 = 40ms
// C should be processed in 40 + 0 = 40ms
// D should be processed in 40 + 10 = 50ms
// 0 |-- A(10) -- B(30) -- C(0) -- D(10)

// adding E which should be processed in 25ms

// we need to insert i beetween A and B in order to have :
// 0 |-- A(10) -- E(15) -- B(15) -- C(0) -- D(10)

void z_tqueue_schedule(struct titem **root, struct titem *item)
{
    struct titem **prev_next_p = root;
    while (*prev_next_p != NULL) {
        /* next of previous become current */
        struct titem *p_current = *prev_next_p;

        /* if new element expires after we go to next.
         *
         * Note: if two items expire at the same time, the item that was inserted first
         * will be processed first. This justify the "<=" in the condition.
         */
        if (p_current->delay_shift <= item->delay_shift) {
            item->delay_shift -= p_current->delay_shift;
            prev_next_p = &(p_current->next);
        } else {
            /* if current element expire before or at the same time,
             * we insert the new item
             */
            item->next = p_current;
            p_current->delay_shift -= item->delay_shift;
            break;
        }
    }
    *prev_next_p = item;
}

void tqueue_schedule(struct titem **root, struct titem *item, k_delta_t timeout)
{
    if (item == NULL)
        return;

    /* last item doesn't have a "next" item */
    item->next    = NULL;
    item->timeout = timeout;

    z_tqueue_schedule(root, item);
}

void tqueue_shift(struct titem **root, k_delta_t time_passed)
{
    struct titem **prev_next_p = root;
    while (*prev_next_p != NULL) {
        /* next of previous become current */
        struct titem *p_current = *prev_next_p;

        if (p_current->delay_shift <= time_passed) {
            /* if item delay_shift is different from 0 */
            if (p_current->delay_shift != 0) {
                time_passed -= p_current->delay_shift;
                p_current->delay_shift = 0;
            }
            prev_next_p = &(p_current->next);
        } else {
            p_current->delay_shift -= time_passed;
            break;
        }
    }
}

struct titem *tqueue_pop(struct titem **root)
{
    struct titem *item = NULL;
    /* pop the first item if expired */
    if ((*root != NULL) && ((*root)->delay_shift == 0)) {
        item  = *root;
        *root = (*root)->next;
    }
    return item;
}

struct titem *tqueue_pop_reschedule(struct titem **root, k_delta_t timeout)
{
    struct titem *item = tqueue_pop(root);
    if (item != NULL) {
        tqueue_schedule(root, item, timeout);
    }
    return item;
}

int8_t tqueue_reschedule(struct titem **root, struct titem *item, k_delta_t timeout)
{
    if (!item || !root)
        return -EINVAL;

    struct titem **pp_next;
    struct titem *p_current;

    struct titem **pp_found  = NULL; // where the item was found
    struct titem **pp_insert = NULL; // where to insert the item on rescheduling

    for (pp_next = root; (p_current = *pp_next) != NULL; pp_next = &(p_current->next)) {
        if (p_current == item)
            pp_found = pp_next;

        // check where to reschedule
        if (!pp_insert) {
            if (p_current->delay_shift > timeout) {
                pp_insert = pp_next;
            } else {
                timeout -= p_current->delay_shift;
            }
        }

        if (pp_found && pp_insert)
            break;
    }

    if (!pp_found)
        return -ENOENT;

    if (!pp_insert)
        pp_insert = pp_next;

    if ((*pp_found)->next != NULL) {
        (*pp_found)->next->delay_shift += item->delay_shift;
    }

    // remove the item
    *pp_found = item->next;

    // reschedule de item
    item->delay_shift = timeout;
    item->next        = *pp_insert;
    if (*pp_insert)
        (*pp_insert)->delay_shift -= timeout;
    *pp_insert = item;

    return 0;
}

void tqueue_remove(struct titem **root, struct titem *item)
{
    struct titem **prev_next_p = root;
    while (*prev_next_p != NULL) {
        struct titem *p_current = *prev_next_p;
        if (p_current == item) {
            *prev_next_p = p_current->next;

            /* add removed item remaining time
             * to the next item if exists */
            if (p_current->next != NULL) {
                p_current->next->delay_shift += item->delay_shift;
            }

            item->next = NULL;
            break;
        }
        prev_next_p = &(p_current->next);
    }
}