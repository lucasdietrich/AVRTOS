#ifndef _IDLE_H_
#define _IDLE_H_

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#if KERNEL_ALLOW_INTERRUPT_YIELD
#define K_TRHEAD_IDLE_STACK_SIZE 2*K_THREAD_STACK_MIN_SIZE + 10u + KERNEL_THREAD_IDLE_ADD_STACK
#else
#define K_TRHEAD_IDLE_STACK_SIZE 1*K_THREAD_STACK_MIN_SIZE + 10u + KERNEL_THREAD_IDLE_ADD_STACK
#endif

/*___________________________________________________________________________*/


extern struct thread_t _k_idle;

void _k_idle_entry(void* context);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif