#ifndef _CLIST_H
#define _CLIST_H

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/*___________________________________________________________________________*/

/**
 * @brief Advanced doubly linked list data structure
 * 
 * push_front/push_pack/dlist_queue/dlist_dequeue are all O(1)
 * 
 * - n : number of items in the list
 * 
 * dlist_count is O(n)
 */

/**
 * - a ditem is already in the list if next != null and prev != null
 * - a ditem is not in the list if next == null && prev == null
 * - poping the last element of the runqueue doesn't have no any effect
 */

/*___________________________________________________________________________*/

struct ditem
{
    union
    {
        struct ditem *next;
        struct ditem *head;
    };
    union
    {
        struct ditem *prev;
        struct ditem *tail;
    };
};

#define DITEM_INIT(self)  \
    {                     \
        {                 \
            .next = self, \
        },                \
        {                 \
            .prev = self, \
        }                 \
    }

#define DLIST_INIT(dlist)   \
    {                       \
        {                   \
            .head = &dlist, \
        },                  \
        {                   \
            .tail = &dlist, \
        }                   \
    }

#define DITEM_INIT_NULL() DITEM_INIT(NULL)

#define DEFINE_DLIST(list_name) struct ditem list_name = DLIST_INIT(list_name)

#define DITEM_VALID(dlist, item) (dlist != item)

/*___________________________________________________________________________*/

void push_front(struct ditem *dlist, struct ditem *item);

void push_back(struct ditem *dlist, struct ditem *item);

struct ditem * pop_front(struct ditem *dlist);

struct ditem * pop_back(struct ditem *dlist);

void pop(struct ditem *item);

/*___________________________________________________________________________*/

void dlist_init(struct ditem *dlist);

void dlist_queue(struct ditem *dlist, struct ditem *item);

struct ditem * dlist_dequeue(struct ditem *dlist);

void dlist_remove(struct ditem *item);

bool dlist_is_empty(struct ditem *dlist);

void dlist_empty(struct ditem *dlist);

void dlist_ref(struct ditem *dlist);

uint8_t dlist_count(struct ditem *dlist);

/*___________________________________________________________________________*/

struct ditem * push_ref(struct ditem **ref, struct ditem *item);

struct ditem * pop_ref(struct ditem **ref);

struct ditem * ref_requeue(struct ditem **ref);

struct ditem * ref_forward_tail(struct ditem **ref);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif