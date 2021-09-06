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

void pop(struct ditem *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
}

struct ditem * pop_front(struct ditem *ref)
{
    struct ditem * item = ref->next;
    ref->next = item->next;
    item->next->prev = ref;
    return item;
}

struct ditem * pop_back(struct ditem *ref)
{
    struct ditem * item = ref->prev;
    ref->prev = item->prev;
    item->prev->next = ref;
    return item;
}

struct ditem * pop_ref(struct ditem **ref)
{
    *ref = (*ref)->next;
    return pop_back(*ref);
}

// output ref
struct ditem * safe_pop_ref(struct ditem **ref)
{
    struct ditem *ret = NULL;
    if (*ref != NULL)
    {
        if ((*ref)->next == *ref)
        {
            ret = *ref;
            *ref = NULL;
        }
        else
        {
            ret = pop_back(*ref);
        }
    }
    return ret;
}

// ref become item
struct ditem * push_ref(struct ditem **ref, struct ditem *item)
{
    push_front(*ref, item);
    return *ref = (*ref)->next;
}

struct ditem * safe_push_ref(struct ditem **ref, struct ditem *item)
{
    if (*ref == NULL)
    {
        *ref = item;
    }
    else
    {
        push_front(*ref, item);
        *ref = (*ref)->next;
    }
    return *ref;
}

void dlist_ref(struct ditem *ref)
{
    *ref = (struct ditem)
    {
        ref,
        ref
    };
}

// move top item to tail
struct ditem * requeue_top(struct ditem *ref)
{
    struct ditem * item = ref->prev;

    ref->prev = item->prev;
    item->prev->next = ref;

    ref->next->prev = item;
    item->next = ref->next;

    ref->next = item;
    item->prev = ref;

    return item;
}

struct ditem * forward_tail(struct ditem *ref)
{
    struct ditem * item = ref->next;

    ref->next = item->next;
    item->next->prev = ref;

    ref->prev->next = item;
    item->prev = ref->prev;
    
    ref->prev = item;
    item->next = ref;

    return item;
}

struct ditem * ref_requeue(struct ditem **ref)
{
    return *ref = (*ref)->next;
}


struct ditem * ref_forward_tail(struct ditem **ref)
{
    return *ref = (*ref)->prev;
}

/*___________________________________________________________________________*/

void dlist_queue(struct ditem **ref, struct ditem * item)
{
    if (*ref == NULL)
    {
        *ref = item;
        dlist_ref(*ref);
    }
    else
    {
        push_back(*ref, item);
    }
}

struct ditem * dlist_dequeue(struct ditem **ref)
{
    struct ditem * pop = NULL;
    if (*ref != NULL)
    {
        if ((*ref)->next == *ref) // == (*ref)->prev
        {
            pop = *ref;
            *ref = NULL;
        }
        else
        {
            pop = pop_ref(ref);
        }
    }
    return pop;
}

void dlist_remove(struct ditem **ref, struct ditem *item)
{
    if ((*ref == item) && (item->next == item))
    {
        *ref = NULL;
    }
    else if (*ref == item)
    {
        pop_ref(ref);
    }
    else
    {
        pop(item);
    }
}

/*___________________________________________________________________________*/