#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct qitem
{
    struct qitem * next;
};

#define DEFINE_QUEUE(ref_name) struct qitem *ref_name = {NULL}

/*___________________________________________________________________________*/

void queue(struct qitem ** const p_root, struct qitem *const qitem);

struct qitem * dequeue(struct qitem ** const p_root);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif