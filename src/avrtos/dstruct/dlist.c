#include "dlist.h"

/*___________________________________________________________________________*/

void push_front(struct ditem *ref, struct ditem *item)
{
    ref->next->prev = item;
    item->next = ref->next;
    ref->next = item;
    item->prev = ref;
}

void push_back(struct ditem *ref, struct ditem *item)
{
    ref->prev->next = item;
    item->prev = ref->prev;
    ref->prev = item;
    item->next = ref;
}

struct ditem * pop_front(struct ditem *ref)
{
    struct ditem * item = ref;
    item = ref->next;
    ref->next = item->next;
    item->next->prev = ref;
    return item;
}

struct ditem * pop_back(struct ditem *ref)
{
    struct ditem * item = ref;
    item = ref->prev;
    ref->prev = item->prev;
    item->prev->next = ref;
    return item;
}

void dlist_ref(struct ditem *ref)
{
    *ref = (struct ditem)
    {
        ref,
        ref
    };
}

void dlist_queue(struct ditem *ref, struct ditem * item)
{
    return push_front(ref, item);
}

struct ditem * dlist_dequeue(struct ditem *ref)
{
    return pop_back(ref);
}

/*___________________________________________________________________________*/