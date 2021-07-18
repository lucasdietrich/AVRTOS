#ifndef _CLIST_H
#define _CLIST_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct ditem
{
    struct ditem *prev;
    struct ditem *next;
};

#define DEFINE_DLIST(ref_name)  struct ditem ref_name = {&ref_name, &ref_name}

/*___________________________________________________________________________*/

void push_front(struct ditem *ref, struct ditem *item);

void push_back(struct ditem *ref, struct ditem *item);

void push_ref(struct ditem **ref, struct ditem *item);

struct ditem * pop_front(struct ditem *ref);

struct ditem * pop_back(struct ditem *ref);

struct ditem * pop_ref(struct ditem **ref);

void dlist_ref(struct ditem *ref);

struct ditem * requeue_top(struct ditem *ref);

struct ditem * forward_tail(struct ditem *ref);

struct ditem * ref_requeue_top(struct ditem **ref);

struct ditem * ref_forward_tail(struct ditem **ref);

void dlist_queue(struct ditem *ref, struct ditem * item);

struct ditem * dlist_dequeue(struct ditem *ref);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif