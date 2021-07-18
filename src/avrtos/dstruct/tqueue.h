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

#define DEFINE_TQUEUE(ref_name) struct titem *ref_name = NULL

/*___________________________________________________________________________*/

void tqueue_schedule(struct titem **root, struct titem *item);

void tqueue_shift(struct titem **root, k_delta_ms_t time_passed);

struct titem * tqueue_pop(struct titem **root);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif