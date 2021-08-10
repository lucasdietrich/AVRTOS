#ifndef _AVRTOS_TQUEUE_H
#define _AVRTOS_TQUEUE_H

#include "../defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct titem
{
    union
    {
        k_delta_ms_t delay_shift;
        k_delta_ms_t abs_delay;
        k_delta_ms_t timeout;
    };
    struct titem* next;
};

#define DEFINE_TQUEUE(name) struct titem *name = NULL
#define INIT_TITEM(timeout_ms)       \
    {                                \
        {                            \
            .timeout = timeout_ms,   \
        },                           \
        .next = NULL                 \
    }
#define DEFINE_TITEM(name) struct titem name = INIT_TITEM(name, timeout_ms)

/*___________________________________________________________________________*/

void _tqueue_schedule(struct titem **root, struct titem *item);

/**
 * @brief If the new event has the same timeout as another event already in the list, it will be added after it
 * 
 * @param root 
 * @param item 
 * @param timeout 
 */
void tqueue_schedule(struct titem **root, struct titem *item, k_delta_ms_t timeout);

void tqueue_shift(struct titem **root, k_delta_ms_t time_passed);

// doesn't set popped item->next to NULL
struct titem * tqueue_pop(struct titem **root);

// set item->next to NULL
void tqueue_remove(struct titem **root, struct titem *item);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif