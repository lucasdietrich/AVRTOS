#ifndef _AVRTOS_CANARIES_H_
#define _AVRTOS_CANARIES_H_

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

void _k_init_thread_canaries(void);

void* _k_thread_canaries(struct thread_t *th);

/*___________________________________________________________________________*/

void print_thread_canaries(struct thread_t *th);

void print_current_canaries(void);

void dump_threads_canaries(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif