#include "queue.h"

/*___________________________________________________________________________*/

void queue(struct item ** const p_root, struct item *const item)
{
    struct item **p_next = p_root;
    while (*p_next != NULL)
    {
        p_next = &(*p_next)->next;
    }
    item->next = NULL;
    *p_next = item;
}

struct item * dequeue(struct item ** const p_root)
{
    struct item *dequeued = *p_root;
    if (dequeued != NULL)
    {
        *p_root = (*p_root)->next;
    }
    return dequeued;
}

/*___________________________________________________________________________*/