/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dlist.h"

void dlist_init(struct dnode *list)
{
    list->head = list;
    list->tail = list;
}

void dlist_append(struct dnode *list, struct dnode *node)
{
    struct dnode *const tail = list->tail;

    // link with tail
    tail->next = node;
    node->prev = tail;

    // link with head
    list->tail = node;
    node->next = list;
}

void dlist_prepend(struct dnode *list, struct dnode *node)
{
    struct dnode *const head = list->head;

    // link with head
    node->prev = list;
    node->next = head;

    // link with successor
    head->prev = node;
    list->head = node;
}

void dlist_insert(struct dnode *successor, struct dnode *node)
{
    struct dnode *const prev = successor->prev;

    // link with predecessor
    prev->next = node;
    node->prev = prev;

    // link with successor
    node->next      = successor;
    successor->prev = node;
}

void dlist_remove(struct dnode *node)
{
    struct dnode *const prev = node->prev;
    struct dnode *const next = node->next;

    prev->next = next;
    next->prev = prev;
}

struct dnode *dlist_get(struct dnode *list)
{
    struct dnode *const item = list->head;

    dlist_remove(item);

    return item;
}

bool dlist_is_empty(struct dnode *list)
{
    return DLIST_EMPTY(list);
}

uint8_t dlist_count(struct dnode *list)
{
    uint8_t count     = 0u;
    struct dnode *cur = list->head;
    while (cur != list) {
        count++;
        cur = cur->next;
    }
    return count;
}