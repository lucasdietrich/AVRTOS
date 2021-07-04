#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stdint.h>

#define SCHEDULER_PRECISION_U32 1
#define SCHEDULER_PRECISION_U16 0
#define SCHEDULER_PRECISION SCHEDULER_PRECISION_U16

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#if SCHEDULER_PRECISION == SCHEDULER_PRECISION_U32
    typedef uint32_t k_delta_ms_t;
#else
    typedef uint16_t k_delta_ms_t;
#endif

typedef struct
{
    k_delta_ms_t delay;
} k_timeout_t;

#define K_MSEC(delay) ((k_timeout_t){delay})
#define K_NO_WAIT(delay) ((k_timeout_t){0})
#define K_FOREVER(delay) ((k_timeout_t){-1})

struct k_scheduled_item_t
{
    union
    {
        k_delta_ms_t delay_shift;
        k_delta_ms_t abs_delay;
    };
    struct k_scheduled_item_t* next;

    void* p;
};

/*___________________________________________________________________________*/

struct k_scheduled_item_t* _k_schedule_get_root(void);

void _k_schedule_submit(struct k_scheduled_item_t *new_item);

void _k_schedule_time_passed(k_delta_ms_t delta);

struct k_scheduled_item_t * _k_schedule_pop_first_expired();

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif