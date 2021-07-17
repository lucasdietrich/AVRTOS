#include "queue.h"

/*___________________________________________________________________________*/

void queue(struct celem ** const p_root, struct celem *const elem)
{
    struct celem **p_next = p_root;
    while (*p_next != NULL)
    {
        p_next = &(*p_next)->next;
    }
    elem->next = NULL;
    *p_next = elem;
}

struct celem * dequeue(struct celem ** const p_root)
{
    struct celem *dequeued = *p_root;
    if (dequeued != NULL)
    {
        *p_root = (*p_root)->next;
    }
    return dequeued;
}

/*___________________________________________________________________________*/