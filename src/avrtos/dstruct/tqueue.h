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

/**
 * @brief Schedule an event by adding it to the list.
 * If the new event has the same timeout as another event already in the list, it will be added after it
 * 
 * Assumptions :
 *  - root is not null
 * 
 * @param root 
 * @param item 
 */
void _tqueue_schedule(struct titem **root, struct titem *item);

/**
 * @see _tqueue_schedule
 * 
 * Assumptions :
 *  - root is not null
 *  - item is not null
 * 
 * @param root 
 * @param item 
 * @param timeout 
 */
void tqueue_schedule(struct titem **root, struct titem *item, k_delta_ms_t timeout);

/**
 * @brief Shift the queue time of {time_passe}
 * 
 * Assumptions : 
 *  - time_passed is not null
 *  - root is not null
 * 
 * @param root 
 * @param time_passed 
 */
void tqueue_shift(struct titem **root, k_delta_ms_t time_passed);

/**
 * @brief Pop an item from the time queue.
 * 
 * Note: this function doesn't set the poped item->next parameter to null.
 * 
 * Assumptions: 
 * - root is not null
 * 
 * @param root 
 * @return struct titem* 
 */
struct titem * tqueue_pop(struct titem **root);

// set item->next to NULL
/**
 * @brief Remove an item from the time queue.
 * Item->next attribute is set to null if found.
 * 
 * Assumptions :
 *  - root is not null
 *  - item is not null
 * 
 * @param root 
 * @param item Item to remove if exists in the time queue.
 */
void tqueue_remove(struct titem **root, struct titem *item);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif