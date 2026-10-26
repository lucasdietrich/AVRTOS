/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_EVQ_H_
#define _AVRTOS_EVQ_H_

#include <stdbool.h>
#include <stdint.h>

#include "dstruct/tqueue.h"
#include "dstruct/dlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define K_KERNEL_EVQ (NULL)

/**
 * @brief Event queue structure.
 *
 * This structure holds the first event in the queue, which is used to manage
 * scheduled events in a time-ordered manner.
 */
struct k_event_q {
    struct titem *first; ///< Pointer to the first event in the queue.
    struct dnode waitqueue; ///< Wait queue for threads waiting for events.
};

/**
 * @brief Macro to initialize the event queue.
 */
#define Z_EVENT_Q_INIT(_name)                                                            \
    {                                                                                    \
        .first     = NULL,                                                               \
        .waitqueue = DLIST_INIT(_name.waitqueue),                                        \
    }

/**
 * @brief Macro to define and initialize an event queue.
 *
 * This macro defines a global event queue named `name` and initializes it using
 * `Z_EVENT_Q_INIT()`.
 */
#define K_EVENT_Q_DEFINE(name) struct k_event_q name = Z_EVENT_Q_INIT(name)

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_EVQ_H_ */
