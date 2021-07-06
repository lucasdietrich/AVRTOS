#ifndef _AVRTOS_XQUEUE_H
#define _AVRTOS_XQUEUE_H

#include <stdint.h>
#include <stddef.h>

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct k_xqueue_item_t
{
    union
    {
        k_delta_ms_t delay_shift;
        k_delta_ms_t abs_delay;
    };
    struct k_xqueue_item_t* next;

    void* p;
};

#define K_XQUEUE_DEFINE(root) struct k_xqueue_item_t * root = NULL;

/*___________________________________________________________________________*/

void k_xqueue_schedule(struct k_xqueue_item_t **root, struct k_xqueue_item_t *new_item);

void k_xqueue_shift(struct k_xqueue_item_t **root, k_delta_ms_t delta);

struct k_xqueue_item_t *k_xqueue_pop(struct k_xqueue_item_t **root);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif