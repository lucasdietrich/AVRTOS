#ifndef _CLIST_H
#define _CLIST_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct celem
{
    struct celem * next;
};

#define CLIST(ref_name) (struct celem ref_name = {NULL})

/*___________________________________________________________________________*/

void queue(struct celem ** const p_root, struct celem *const item);

struct celem * dequeue(struct celem ** const p_root);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif