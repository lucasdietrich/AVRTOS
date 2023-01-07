/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _OQUEUE_H
#define _OQUEUE_H

#include "queue.h"

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

/**
 * @brief Optimization choosed :
 *  - OQUEUE_OPT_DEQUEUE : optimize odequeue function by not setting
 *      oref->tail value to NULL when we dequeue the last item
 *  - OQUEUE_OPT_PEEK : optimize opeek_tail function, by setting
 *      oref->tail value to NULL when we dequeue the last item
 */
#define OQUEUE_OPT_DEQUEUE 0
#define OQUEUE_OPT_PEEK	   1

#define OQUEUE_OPT_TYPE OQUEUE_OPT_DEQUEUE

/*___________________________________________________________________________*/

struct oqref {
	struct qitem *head;
	struct qitem *tail;
};

#define INIT_OQREF()                                                                     \
	{                                                                                \
		.head = NULL, .tail = NULL                                               \
	}
#define DEFINE_OQREF(name) struct oqref name = INIT_OQREF()

/*___________________________________________________________________________*/

void oqref_init(struct oqref *oref);

void oqueue(struct oqref *oref, struct qitem *item);

struct qitem *odequeue(struct oqref *oref);

inline struct qitem *opeek_head(struct oqref *oref)
{
	return oref->head;
}

inline struct qitem *opeek_tail(struct oqref *oref)
{
#if OQUEUE_OPT_TYPE == OQUEUE_OPT_DEQUEUE
	if (oref->head == NULL) {
		return NULL;
	}
#endif

	return oref->tail;
}

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif