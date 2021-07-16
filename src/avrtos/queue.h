#ifndef _AVRTOS_MUTEX_H
#define _AVRTOS_MUTEX_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct item
{
    struct item * next;
    void * p;
};

void queue(struct item ** const p_root, struct item *const item);

struct item * dequeue(struct item ** const p_root);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif