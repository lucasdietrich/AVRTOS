/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dlist.h"

/*___________________________________________________________________________*/

void dlist_init(struct ditem *list)
{
        list->head = list;
        list->tail = list;
}

void dlist_append(struct ditem *list, struct ditem *node)
{
	struct ditem *const tail = list->tail;
	
	tail->next = node;
	node->prev = tail;

	list->tail = node;
	node->next = list;
}

void dlist_prepend(struct ditem *list, struct ditem *node)
{
	struct ditem *const head = list->head;

	node->prev = list;
	node->next = head;
	
	head->prev = node;
	list->head = node;
}

void dlist_insert(struct ditem *successor, struct ditem *node)
{
	struct ditem *const prev = successor->prev;
	
	prev->next = node;
	node->prev = prev;
	
	node->next = successor;
	successor->prev = node;
}

void dlist_remove(struct ditem *node)
{
	struct ditem *const prev = node->prev;
	struct ditem *const next = node->next;
	
	prev->next = next;
	next->prev = prev;
}

struct ditem *dlist_get(struct ditem *list)
{
	struct ditem *const item = list->head;

	dlist_remove(item);

	return item;
}

bool dlist_is_empty(struct ditem *list)
{
	return DLIST_EMPTY(list);
}

uint8_t dlist_count(struct ditem *list)
{
	uint8_t count = 0u;
	struct ditem *cur = list->head;
	while (cur != list) {
		count++;
		cur = cur->next;
	}
	return count;
}