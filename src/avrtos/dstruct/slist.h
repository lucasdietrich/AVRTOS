/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_SYS_SLIST_H_
#define _AVRTOS_SYS_SLIST_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Optimized queue data structure with head/tail entries.
 *
 * queue/dequeue actions are O(1)
 */

/*___________________________________________________________________________*/

struct snode {
	struct snode *next;
};

struct slist {
	struct snode *head;
	struct snode *tail;
};

typedef struct slist slist_t;
typedef struct snode snode_t;

#define SLIST_INIT()                       \
	{                                  \
		.head = NULL, .tail = NULL \
	}
#define SLIST_DEFINE(name) struct slist name = SLIST_INIT()

#define SNODE_INIT()         \
	{                    \
		.next = NULL \
	}

/*___________________________________________________________________________*/

void slist_init(struct slist *list);

void slist_append(struct slist *list, struct snode *node);

struct snode *slist_get(struct slist *list);

inline struct snode *slist_peek_head(struct slist *list)
{
	return list->head;
}

inline struct snode *slist_peek_tail(struct slist *list)
{
	return list->tail;
}

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif