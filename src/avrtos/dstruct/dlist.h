#ifndef _CLIST_H
#define _CLIST_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct delem
{
    struct delem *prev;
    struct delem *next;
};

#define DLIST_REF(ref_name)  (struct delem ref_name = {ref_name, ref_name })   

/*___________________________________________________________________________*/

void push_front(struct delem *ref, struct delem *item);

void push_back(struct delem *ref, struct delem *item);

struct delem * pop_front(struct delem *ref);

struct delem * pop_back(struct delem *ref);

void dlist_ref(struct delem *ref);

void dlist_queue(struct delem *ref, struct delem * item);

struct delem * dlist_dequeue(struct delem *ref);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif