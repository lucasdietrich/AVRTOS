#ifndef _DSTRUCT_DEBUG_H
#define _DSTRUCT_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#include "avrtos/misc/uart.h"

/*___________________________________________________________________________*/

#include "queue.h"

//
// Queue
//
void print_queue(struct qitem *root, void (*qitem_printer)(struct qitem *item));

/*___________________________________________________________________________*/

#include "dlist.h"

//
// Dlist
//
void print_dlist(struct ditem *ref, void (*ditem_printer)(struct ditem *item));

/*___________________________________________________________________________*/


/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif