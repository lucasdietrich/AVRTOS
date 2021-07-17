#include "dlist.h"

/*___________________________________________________________________________*/

void push_front(struct delem *ref, struct delem *item)
{
    ref->next->prev = item;
    item->next = ref->next;
    ref->next = item;
    item->prev = ref;
}

void push_back(struct delem *ref, struct delem *item)
{
    ref->prev->next = item;
    item->prev = ref->prev;
    ref->prev = item;
    item->next = ref;
}

struct delem * pop_front(struct delem *ref)
{
    struct delem * item = ref;
    item = ref->next;
    ref->next = item->next;
    item->next->prev = ref;
    return item;
}

struct delem * pop_back(struct delem *ref)
{
    struct delem * item = ref;
    item = ref->prev;
    ref->prev = item->prev;
    item->prev->next = ref;
    return item;
}

void dlist_ref(struct delem *ref)
{
    *ref = (struct delem)
    {
        ref,
        ref
    };
}

void dlist_queue(struct delem *ref, struct delem * item)
{
    return push_front(ref, item);
}

struct delem * dlist_dequeue(struct delem *ref)
{
    return pop_back(ref);
}

/*___________________________________________________________________________*/