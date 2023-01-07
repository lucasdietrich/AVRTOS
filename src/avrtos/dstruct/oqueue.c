/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "oqueue.h"

/*___________________________________________________________________________*/

void oqref_init(struct oqref *oref)
{
	oref->head = NULL;
	oref->tail = NULL;
}

void oqueue(struct oqref *oref, struct qitem *item)
{
	/* safely set next item pointer to NULL */
	item->next = NULL;

	if (oref->head == NULL) {
		/* case queue empty */
		oref->head = item;
	} else {
		/* case queue not empty */
		oref->tail->next = item;
	}
	oref->tail = item;
}

struct qitem *odequeue(struct oqref *oref)
{
	struct qitem *dequeued = oref->head;
	if (oref->head != NULL) {
		oref->head = dequeued->next;

		/* in OQUEUE_OPT_PEEK optimization mode
		 * it's not necessary to set ref->tail to NULL
		 * if dequeued->next is null
		 */
#if OQUEUE_OPT_TYPE == OQUEUE_OPT_PEEK

		if (oref->head == NULL) {
			oref->tail = NULL;
		}
#endif
	}
	return dequeued;
}

/*___________________________________________________________________________*/