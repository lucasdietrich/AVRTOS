#ifndef _AVRTOS_KERNEL_H
#define _AVRTOS_KERNEL_H

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif


/*___________________________________________________________________________*/

void _k_kernel_init(void);

void k_queue(struct thread_t *th);

/**
 * @brief Choice the next thread to be executed
 * 
 * This function is called in k_yield function
 * 
 * @return struct thread_t* : next thread to be executed
 */
struct thread_t *_k_scheduler(void);

/**
 * @brief Release the CPU
 * 
 * This function can be called from either a cooperative thread or a premptive thread to tell the kernel that it is releasing the CPU
 * this function will call the scheduler (_k_scheduler) which will determine the next thread to be executed.
 * 
 * Cooperative threads must use this function in order to release the CPU
 * 
 * This function will return in the new main thread
 */
void _k_yield(void);

void k_yield(void);

/*___________________________________________________________________________*/

void _k_system_shift(void);

void k_sleep(k_timeout_t timeout);

void k_cpu_idle(void);

/*___________________________________________________________________________*/

uint8_t mutex_lock_wait(mutex_t *mutex, k_timeout_t timeout);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 

