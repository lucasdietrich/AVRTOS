/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tdqueue.h"

#include "tqueue.h"

// A should be processed in 10 ms
// B should be processed in 10 + 30 = 40ms
// C should be processed in 40 + 0 = 40ms
// D should be processed in 40 + 10 = 50ms
// 0 |-- A(10) -- B(30) -- C(0) -- D(10)

// adding E which should be processed in 25ms

// we need to insert i beetween A and B in order to have :
// 0 |-- A(10) -- E(15) -- B(15) -- C(0) -- D(10)

void z_tdqueue_schedule(dlist_t *tqueue, struct tditem *item)
{
    // Get the first item in the list
    struct dnode *p_node = tqueue->head;

    while (p_node != tqueue) {
        /* next of previous become current */
        struct tditem *p_item = CONTAINER_OF(p_node, struct tditem, node);

        /* if new element expires after we go to next.
         *
         * Note: if two items expire at the same time, the item that was inserted first
         * will be processed first. This justify the "<=" in the condition.
         */
        if (p_item->delay_shift <= item->delay_shift) {
            item->delay_shift -= p_item->delay_shift;
            p_node = p_node->next;
        } else {
            /* if current element expire before or at the same time,
             * we insert the new item
             */
            dlist_insert(p_node, &item->node);
            p_item->delay_shift -= item->delay_shift;

            return; /* exit */
        }
    }

    dlist_insert(p_node, &item->node);
}

void tdqueue_schedule(dlist_t *tqueue, struct tditem *item, k_delta_t timeout)
{
    if (item == NULL) return;

    dlist_init(&item->node);
    item->timeout = timeout;

    z_tdqueue_schedule(tqueue, item);
}

void tdqueue_shift(dlist_t *tqueue, k_delta_t time_passed)
{
    // Get the first item in the list
    struct dnode *p_node = tqueue->head;

    while (p_node != tqueue) {
        /* next of previous become current */
        struct tditem *p_item = CONTAINER_OF(p_node, struct tditem, node);

        if (p_item->delay_shift <= time_passed) {
            /* if item delay_shift is different from 0 */
            if (p_item->delay_shift != 0) {
                time_passed -= p_item->delay_shift;
                p_item->delay_shift = 0;
            }
            p_node = p_node->next;
        } else {
            p_item->delay_shift -= time_passed;
            break;
        }
    }
}

struct tditem *tdqueue_pop(dlist_t *tqueue)
{
    /* Get head */
    struct tditem *head = CONTAINER_OF(tqueue->head, struct tditem, node);

    if (head->delay_shift == 0) {
        dlist_remove(&head->node);
    } else {
        head = NULL;
    }

    return head;
}

struct tditem *tdqueue_pop_reschedule(dlist_t *tqueue, k_delta_t timeout)
{
    struct tditem *item = tdqueue_pop(tqueue);
    if (item != NULL) {
        tdqueue_schedule(tqueue, item, timeout);
    }
    return item;
}

void tdqueue_remove(dlist_t *tqueue, struct tditem *item)
{
    struct dnode *const prev = item->node.prev;
    struct dnode *const next = item->node.next;

    prev->next = next;
    next->prev = prev;

    /* If next doesn't point to the list, it means that the item is not the last one
     * and we need to update the delay_shift of the next item.
     */
    if (next != tqueue) {
        // Update the delay_shift of the successor
        struct tditem *successor = CONTAINER_OF(next, struct tditem, node);
        successor->delay_shift += item->delay_shift;
    }
}