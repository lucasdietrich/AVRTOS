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

void queue(struct qitem ** p_root, struct qitem * qitem);

struct qitem * dequeue(struct qitem ** p_root);

// unqueue

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif