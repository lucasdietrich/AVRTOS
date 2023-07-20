/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_STATS_H
#define _AVRTOS_STATS_H

#include <stdint.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Type for any counter defined in the stats API */
typedef uint16_t z_counter_t;
typedef uint32_t z_ticks_counter_t;

#if CONFIG_KERNEL_STATS

#define Z_STATS_OBJ_CLEAR(_type, _obj) ((_obj)->_stats = (_type){0u})
#define Z_STATS_OBJ_INC(_obj, _field) ((_obj)->_stats._field++)
#define Z_STATS_OBJ_ADD(_obj, _field, _val) ((_obj)->_stats._field += (_val))
#define Z_STATS_OBJ_GET(_obj, _field) ((_obj)->_stats._field)

#else

#define Z_STATS_OBJ_CLEAR(_type, _obj) ((void)0)
#define Z_STATS_OBJ_INC(_obj, _field) ((void)0)
#define Z_STATS_OBJ_ADD(_obj, _field, _val) ((void)0)
#define Z_STATS_OBJ_GET(_obj, _field) (0u)

#endif

struct z_stats_thread {
        z_counter_t preempted; /* Number of times the thread was preempted */
        z_counter_t yield; /* Number of times the thread yielded */
        z_counter_t swap_data; /* Number of times the thread data was swapped */
        z_counter_t pending; /* Number of times the thread was pending on an object */
};

struct z_stats_kernel {
        
};

struct z_stats_workq {
        z_counter_t processed; /* Number of work items processed */
};

#define Z_STATS_WORKQ_CLEAR(_workq) Z_STATS_OBJ_CLEAR(struct z_stats_workq, _workq)
#define Z_STATS_WORKQ_PROC(_workq) Z_STATS_OBJ_INC(_workq, processed)

struct  z_stats_sem {
        z_counter_t gives;
        z_counter_t takes;
        z_counter_t pendings;
};

#define Z_STATS_SEM_CLEAR(_sem) Z_STATS_OBJ_CLEAR(struct z_stats_sem, _sem)
#define Z_STATS_SEM_GIVE(_sem) Z_STATS_OBJ_INC(_sem, gives)
#define Z_STATS_SEM_TAKE(_sem) Z_STATS_OBJ_INC(_sem, takes)
#define Z_STATS_SEM_PEND(_sem) Z_STATS_OBJ_INC(_sem, pendings)

struct z_stats_mutex {
        z_counter_t locks;
        z_counter_t max_pending;
};


#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_STATS_H */