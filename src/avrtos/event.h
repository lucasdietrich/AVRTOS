#ifndef _AVRTOS_EVENT_H_
#define _AVRTOS_EVENT_H_

#include <avrtos/multithreading.h>

#include <stdbool.h>
#include <stdint.h>

#include <avrtos/dstruct/tqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct k_event;

typedef void (*k_event_handler_t)(struct k_event*);

struct k_event
{
    struct titem tie;
    k_event_handler_t handler;
};


#define K_EVENT_INIT(hdlr)           \
    {                                \
    	.tie = INIT_TITEM_DEFAULT(), \
        .handler = hdlr              \
    }
#define K_EVENT_DEFINE(name, hdlr) struct k_event name = K_EVENT_INIT(hdlr)

/*___________________________________________________________________________*/

/**
 * @brief Initialize an event.
 * 
 * @param event 
 * @param handler 
 * @return int 
 */
K_NOINLINE int k_event_init(struct k_event *event, k_event_handler_t handler);

/**
 * @brief Schedule the event to be triggered after timeout (in ms).
 * 
 * Can be called from an interrupt.
 * 
 * @param event 
 * @param timeout 
 * @return int 
 */
K_NOINLINE int k_event_schedule(struct k_event *event, k_timeout_t timeout);

/**
 * @brief Unschedule the event.
 * 
 * Can be called from an interrupt.
 * 
 * @param event 
 * @return int 
 */
K_NOINLINE int k_event_cancel(struct k_event *event);

/**
 * @brief Internal function shifting the event queue and executing the handlers
 * for the events that have expired.
 * 
 * The call periodicity is constant and equals KERNEL_TIME_SLICE.
 */
K_NOINLINE void _k_event_q_process(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_IO_H_ */