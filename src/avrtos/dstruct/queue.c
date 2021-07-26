#include "queue.h"

/*___________________________________________________________________________*/

void queue(struct qitem ** p_root, struct qitem * elem)
{
    struct qitem **p_next = p_root;
    while (*p_next != NULL)
    {
        p_next = &(*p_next)->next;
    }
    elem->next = NULL;
    *p_next = elem;
}

struct qitem * dequeue(struct qitem ** p_root)
{
    struct qitem *dequeued = *p_root;
    if (dequeued != NULL)
    {
        *p_root = (*p_root)->next;
    }
    return dequeued;
}

/*___________________________________________________________________________*/