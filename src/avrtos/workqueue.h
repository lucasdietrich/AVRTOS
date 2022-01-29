#ifndef _AVRTOS_WORKQUEUE
#define _AVRTOS_WORKQUEUE

#include <stddef.h>

#include <avrtos/kernel.h>
#include <avrtos/fifo.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct k_work;

typedef void (*k_work_handler_t)(struct k_work *);

struct k_work
{
    struct qitem _tie;
    k_work_handler_t handler;
};

#define K_WORK_INIT(work_handler) \
    {                             \
        ._tie = INIT_QITEM(),     \
        .handler = work_handler,  \
    }

#define K_WORK_DEFINE(work_name, work_handler) \
    struct k_work work_name = K_WORK_INIT(work_handler)

/*___________________________________________________________________________*/

struct k_workqueue
{
        struct k_fifo q;
        union {
                uint8_t flags;
                struct {
                        uint8_t yieldeach: 1;
                };
        };
};

#define k_workq k_workqueue


#define K_WORKQUEUE_DEFINE(name, stack_size, prio_flags, symbol) \
    struct k_workqueue name =                                    \
        {                                                        \
            .q = K_FIFO_INIT(name.q),                            \
            .flags = 0u,                                         \
        };                                                       \
    K_THREAD_DEFINE(                                             \
        _k_workq_##name,                                         \
        _k_workqueue_entry, stack_size,                          \
        prio_flags,                                              \
        &name,                                                   \
        symbol)                                                  \


/*___________________________________________________________________________*/

//
// Workqueue internal
//

K_NOINLINE void _k_workqueue_entry(struct k_workqueue *const workqueue);

/*___________________________________________________________________________*/

/**
 * @brief Initialize a workqueue item at runtime.
 * k_work items can also be defined at compilation time using K_WORK_DEFINE
 * 
 * @param work work item
 * @param handler handler 
 */
K_NOINLINE void k_work_init(struct k_work *work, k_work_handler_t handler);

/**
 * @brief Tells if the work item is submittable or not.
 * 
 * @param work 
 * @return true if submittable
 * @return false if in a workqueueu fifo
 */
bool k_work_submittable(struct k_work *work);

/**
 * @brief Submit a work item to the desired workqueue.
 * 
 * If the work item has already been queued but didn't get processed, the 
 * work is not sent a new time.
 * 
 * A work item started being processed can be submitted again.
 * 
 * A work item can be sent from an interrupt handler. 
 * With "signals", this is the recommanded way to handle interrupts.
 * 
 * Assumptions :
 *  - workqueue is not null
 *  - work is not null
 *  - work->handler is not null
 * 
 * @param workqueue 
 * @param work 
 */
K_NOINLINE bool k_work_submit(struct k_workqueue *workqueue, struct k_work *work);

/**
 * @brief Configure the workqueue to release the cpu after each work item
 * being processed.
 * 
 * Prevent cooperative thread workqueue from keeping the cpu for too long
 * if a lot of work items need to be processed.
 * 
 * @param workqueue 
 */
K_NOINLINE void k_workqueue_set_yieldeach(struct k_workqueue *workqueue);

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
K_NOINLINE void k_workqueue_clr_yieldeach(struct k_workqueue *workqueue);

/*___________________________________________________________________________*/

//
// System workqueue
//

/**
 * @brief Send a work item to the system workqueue
 * 
 * @param work 
 */
bool k_system_workqueue_submit(struct k_work *work);

/*___________________________________________________________________________*/


#ifdef __cplusplus
}
#endif

#endif