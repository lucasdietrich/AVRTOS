/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "slist.h"

void slist_init(struct slist *list)
{
    list->head = NULL;
    list->tail = NULL;
}

void slist_append(struct slist *list, struct snode *node)
{
    /* safely set next item pointer to NULL */
    node->next = NULL;

    if (list->head == NULL) {
        /* case queue empty */
        list->head = node;
    } else {
        /* case queue not empty */
        list->tail->next = node;
    }
    list->tail = node;
}

struct snode *slist_get(struct slist *list)
{
    struct snode *node = list->head;
    if (list->head != NULL) {
        list->head = node->next;

        if (list->head == NULL) {
            list->tail = NULL;
        }
    }
    return node;
}