#ifndef _AVRTOS_XTQUEUE_H
#define _AVRTOS_XTQUEUE_H

#include "../defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct k_xtqueue_item_t
{
    union
    {
        k_delta_ms_t delay_shift;
        k_delta_ms_t abs_delay;
        k_delta_ms_t timeout;
    };
    struct k_xtqueue_item_t* next;

    void* p_context;
};

#define K_xtqueue_DEFINE(root) struct k_xtqueue_item_t * root = NULL;

/*___________________________________________________________________________*/

void k_xtqueue_schedule(struct k_xtqueue_item_t **root, struct k_xtqueue_item_t *new_item);

void k_xtqueue_shift(struct k_xtqueue_item_t **root, k_delta_ms_t delta);

struct k_xtqueue_item_t *k_xtqueue_pop(struct k_xtqueue_item_t **root);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif