#ifndef _IDLE_H_
#define _IDLE_H_

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

// todo, no keep the idle thread in the runqueue (add it/remove it) when runqueue changes, use runqueue control functions

/*___________________________________________________________________________*/


extern struct thread_t _k_idle;

void _k_idle_entry(void* context);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif