/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Simple queue data structure
 * 
 * - n : number of items in the list
 * 
 * queue action is O(n)
 * dequeue action is O(1)
 * 
 */

/*___________________________________________________________________________*/

struct qitem
{
    struct qitem* next;
};

#define DEFINE_QUEUE(ref_name) struct qitem *ref_name = NULL

#define INIT_QITEM() \
    {                \
        .next = NULL \
    }
#define DEFINE_QITEM(name) struct qitem ref_name = INIT_QITEM()

/*___________________________________________________________________________*/

inline void queue_init(struct qitem **p_root)
{
        *p_root = NULL;
}

void queue(struct qitem **p_root, struct qitem *item);

struct qitem *dequeue(struct qitem **p_root);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif