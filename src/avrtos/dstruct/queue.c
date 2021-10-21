#include "queue.h"

/*___________________________________________________________________________*/

void queue(struct qitem **p_root, struct qitem *item)
{
        item->next = NULL;
        struct qitem **p_next = p_root;
        while (*p_next != NULL) {
                p_next = &(*p_next)->next;
        }
        *p_next = item;
}

struct qitem *dequeue(struct qitem **p_root)
{
        struct qitem *dequeued = *p_root;
        if (dequeued != NULL) {
                *p_root = (*p_root)->next;
        }
        return dequeued;
}

/*___________________________________________________________________________*/