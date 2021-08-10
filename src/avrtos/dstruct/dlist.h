#ifndef _CLIST_H
#define _CLIST_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/*___________________________________________________________________________*/

// a ditem is already in the list if next != null and prev != null
// a ditem is not in the list if next == null && prev == null

// poping the last element of the runqueue doesn't have no any effect

/*___________________________________________________________________________*/

struct ditem
{
    struct ditem *prev;
    struct ditem *next;
};

#define INIT_DITEM(self)           \
    {                              \
        .prev = self,              \
        .next = self               \
    }
#define INIT_DITEM_NULL()           INIT_DITEM(NULL)

#define DEFINE_DLIST(ref_name) struct dlist ref_name = INIT_DITEM(&ref_name)

/*___________________________________________________________________________*/

void push_front(struct ditem *ref, struct ditem *item);

void push_back(struct ditem *ref, struct ditem *item);

struct ditem * push_ref(struct ditem **ref, struct ditem *item);

struct ditem * safe_push_ref(struct ditem **ref, struct ditem *item);

struct ditem * pop_front(struct ditem *ref);

struct ditem * pop_back(struct ditem *ref);

struct ditem * pop_ref(struct ditem **ref);

struct ditem * safe_pop_ref(struct ditem **ref);

void dlist_ref(struct ditem *ref);

struct ditem * requeue_top(struct ditem *ref);

struct ditem * forward_tail(struct ditem *ref);

struct ditem * ref_requeue(struct ditem **ref);

struct ditem * ref_forward_tail(struct ditem **ref);

/*___________________________________________________________________________*/

// Queue, starting at ref, different from push_back and pop_front

void dlist_queue(struct ditem **ref, struct ditem * item);

struct ditem * dlist_dequeue(struct ditem **ref);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif