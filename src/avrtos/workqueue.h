#ifndef _AVRTOS_WORKQUEUE
#define _AVRTOS_WORKQUEUE

#include <stddef.h>

#include "multithreading.h"
#include "dstruct/tqueue.h"


/*___________________________________________________________________________*/

struct k_work;

typedef void (*k_work_handler_t)(struct k_work *);

struct k_work
{
    struct ditem tie;
    k_work_handler_t handler;
};

#define K_WORK_INIT(work_handler) \
    {                             \
        .tie = DITEM_INIT_NULL(), \
        .handler = work_handler,  \
    }

#define K_WORK_DEFINE(work_name, work_handler) \
    static struct k_work work_name = K_WORK_INIT(work_handler)

/*___________________________________________________________________________*/

#define K_WORKQUEUE_IDLE        (1 << 0)

#define K_WORKQUEUE_YIELDEACH   (2 << 0)

struct k_workqueue
{
    struct ditem queue;
    struct k_thread* thread;
    uint8_t flags;
};

#define k_workq k_workqueue

#define _K_WORKQUEUE_THREAD_NAME(name) _k_workq_##name

#define K_WORKQUEUE_DEFINE(name, stack_size, prio_flags, symbol) \
    extern struct k_workqueue name;                              \
    K_THREAD_DEFINE(                                             \
        _K_WORKQUEUE_THREAD_NAME(name),                          \
        _k_workqueue_entry, stack_size,                          \
        prio_flags,                                              \
        &name,                                                   \
        &name,                                                   \
        symbol)                                                  \
    struct k_workqueue name =                                    \
        {                                                        \
            .queue = DLIST_INIT(name.queue),                     \
            .thread = &_K_WORKQUEUE_THREAD_NAME(name),           \
            .flags = 0u,                                         \
    };

/*___________________________________________________________________________*/

//
// Workqueue internal
//

void _k_workqueue_entry(struct k_workqueue *const workqueue);

/*___________________________________________________________________________*/

/**
 * @brief Initialize a workqueue item at runtime.
 * k_work items can also be defined at compilation time using K_WORK_DEFINE
 * 
 * @param work work item
 * @param handler handler 
 */
void k_work_init(struct k_work * work, k_work_handler_t handler);

/**
 * @brief Submit a work item to the desired workqueue.
 * 
 * If the work item has already been queued but didn't get processed, the 
 * work is not sent a new time.
 * 
 * Assumptions :
 *  - workqueue is not null
 *  - work is not null
 *  - work->handler is not null
 * 
 * A work item being processed can be submitted again.
 * 
 * @param workqueue 
 * @param work 
 */
void k_work_submit(struct k_workqueue * workqueue, struct k_work * work);

/**
 * @brief Configure the workqueue to release the cpu after each work item
 * being processed.
 * 
 * Prevent cooperative thread workqueue from keeping the cpu for too long
 * if a lot of work items need to be processed.
 * 
 * @param workqueue 
 */
void k_workqueue_set_yieldeach(struct k_workqueue * workqueue);

/**
 * @brief Configure the workqueue to not release the cpu after each work
 * item if there are more to be processed.
 * 
 * Assumptions :
 *  - work is not null
 *  - work->handler is not null
 * 
 * @see Undo k_workqueue_set_yieldeach
 * 
 * @param workqueue 
 */
void k_workqueue_clr_yieldeach(struct k_workqueue * workqueue);

/*___________________________________________________________________________*/

//
// System workqueue
//

/**
 * @brief Send a work item to the system workqueue
 * 
 * @param work 
 */
void k_system_workqueue_submit(struct k_work * work);

/*___________________________________________________________________________*/


#endif