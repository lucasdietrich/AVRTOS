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


#define K_WORK_INIT(work_handler) { .tie = INIT_DITEM_NULL(), .handler = work_handler }
#define K_WORK_DEFINE(work_name, work_handler) static struct k_work work_name = K_WORK_INIT(work_handler)

/*___________________________________________________________________________*/

#define K_WORKQUEUE_IDLE  (1 << 0)

struct k_workqueue
{
    struct ditem *queue;
    struct k_thread* thread;
    uint8_t flags;
};

#define _K_WORKQUEUE_THREAD_NAME(name) workq_##name

#define K_WORKQUEUE_DEFINE(name, stack_size, prio_flags)                                                             \
    extern struct k_workqueue name;                                                                                  \
    K_THREAD_DEFINE(_K_WORKQUEUE_THREAD_NAME(name), _k_workqueue_entry, stack_size, prio_flags, &name, &name, 'W') \
    struct k_workqueue name = {.queue = NULL, .thread = &_K_WORKQUEUE_THREAD_NAME(name), .flags = 0u};

/*___________________________________________________________________________*/

void _k_workqueue_entry(struct k_workqueue *const workqueue);

/*___________________________________________________________________________*/

void k_work_init(struct k_work * work, k_work_handler_t handler);

void k_work_submit(struct k_workqueue * workqueue, struct k_work * work);

/*___________________________________________________________________________*/

//
// System workqueue
//

void k_system_workqueue_submit(struct k_work * work);

/*___________________________________________________________________________*/


#endif