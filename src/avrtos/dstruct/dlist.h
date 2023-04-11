/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CLIST_H
#define _CLIST_H

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Doubly linked list data structure
 * - O(1) queue/dequeue/insertion/deletion complexity.
 *
 * - n : number of items in the list
 * dlist_count is O(n)
 */

/**
 * - a dnode is already in the list if next != null and prev != null
 * - a dnode is not in the list if next == null && prev == null
 * - poping the last element of the runqueue doesn't have no any effect
 */

/*___________________________________________________________________________*/

struct dnode {
	union {
		struct dnode *next;
		struct dnode *head;
	};
	union {
		struct dnode *prev;
		struct dnode *tail;
	};
};

typedef struct dnode ditem_t;
typedef struct dnode dlist_t;

#define DITEM_INIT(self)              \
	{                             \
		{                     \
			.next = self, \
		},                    \
		{                     \
			.prev = self, \
		}                     \
	}

#define DLIST_INIT(_list)               \
	{                               \
		{                       \
			.head = &_list, \
		},                      \
		{                       \
			.tail = &_list, \
		}                       \
	}

#define DITEM_INIT_NULL() DITEM_INIT(NULL)

#define DEFINE_DLIST(list_name) struct dnode list_name = DLIST_INIT(list_name)

#define DLIST_EMPTY(_list) (_list->head == _list)

#define DITEM_VALID(_list, _node) (_list != _node)
#define DITEM_NEXT(_list, _node)  (_node->next)
#define DITEM_PREV(_list, _node)  (_node->prev)

/*___________________________________________________________________________*/

#define DLIST_FOREACH(_list, _node) \
	for (_node = _list->head; _node != _list; _node = _node->next)

#define DLIST_FOREACH_SAFE(_list, _tmp, _node)                        \
	for (_node = _list->head, _tmp = _list->head; _node != _list; \
	     _node = _node->next, _tmp = _node)

/*___________________________________________________________________________*/

/**
 * @brief Initialize a doubly linked list
 *
 * @param list
 */
void dlist_init(struct dnode *list);

/**
 * @brief Add (queue) a node to the end of a doubly linked list
 *
 * @param list
 * @param node
 */
void dlist_append(struct dnode *list, struct dnode *node);

/**
 * @brief Add (queue) a node to the beginning of a doubly linked list
 *
 * @param list
 * @param node
 */
void dlist_prepend(struct dnode *list, struct dnode *node);

/**
 * @brief Insert a node before a given node
 *
 * @param successor
 * @param node
 */
void dlist_insert(struct dnode *successor, struct dnode *node);

/**
 * @brief Remove a node from a doubly linked list
 *
 * @param node
 */
void dlist_remove(struct dnode *node);

/**
 * @brief Get the first node of a doubly linked list (dequeue)
 *
 * @param list
 * @return struct dnode*
 */
struct dnode *dlist_get(struct dnode *list);

/**
 * @brief Check if a doubly linked list is empty
 *
 * @param list
 * @return true
 * @return false
 */
bool dlist_is_empty(struct dnode *list);

/**
 * @brief Calculate the number of items in a doubly linked list
 *
 * @param list
 * @return uint8_t
 */
uint8_t dlist_count(struct dnode *list);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif