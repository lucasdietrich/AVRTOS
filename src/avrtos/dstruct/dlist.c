#include "dlist.h"

/*___________________________________________________________________________*/

//
// Raw API
//

void push_front(struct ditem *dlist, struct ditem *item)
{
    dlist->head->prev = item;
    item->next = dlist->head;
    dlist->head = item;
    item->prev = dlist;
}

void push_back(struct ditem *dlist, struct ditem *item)
{
    dlist->tail->next = item;
    item->prev = dlist->tail;
    dlist->tail = item;
    item->next = dlist;
}

struct ditem * pop_front(struct ditem *dlist)
{
    struct ditem * item = dlist->head;
    dlist->head = item->next;
    item->next->prev = dlist;
    return item;
}

struct ditem * pop_back(struct ditem *dlist)
{
    struct ditem * item = dlist->tail;
    dlist->tail = item->prev;
    item->prev->next = dlist;
    return item;
}

void pop(struct ditem *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
}

/*___________________________________________________________________________*/

//
// Public dlist API
//
// dlist references to the head and the tail of the doubly-linky-list

void dlist_init(struct ditem *dlist)
{
    dlist->head = dlist;
    dlist->tail = dlist;
}

void dlist_queue(struct ditem *dlist, struct ditem *item)
{
    push_front(dlist, item);
}

struct ditem * dlist_dequeue(struct ditem *dlist)
{
    return pop_back(dlist);
}

void dlist_remove(struct ditem *item)
{
    pop(item);
}

bool dlist_is_empty(struct ditem *dlist)
{
    return dlist->head == dlist;
}

void dlist_empty(struct ditem *dlist)
{
    dlist_init(dlist);
}

void dlist_ref(struct ditem *dlist)
{
    dlist_init(dlist);
}

uint8_t dlist_count(struct ditem *dlist)
{
    uint8_t count = 0u;
    struct ditem *cur = dlist->head;
    while(cur != dlist)
    {
        count++;
        cur = cur->next;
    }
    return count;
}

/*___________________________________________________________________________*/

//
// Public cdlist API
//

// circular dlist, ref is a pointer to the reference of the circular doubly-linky list

struct ditem * push_ref(struct ditem **ref, struct ditem *item)
{
    push_front(*ref, item);
    return *ref = (*ref)->next;
}
struct ditem * pop_ref(struct ditem **ref)
{
    *ref = (*ref)->next;
    return pop_back(*ref);
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