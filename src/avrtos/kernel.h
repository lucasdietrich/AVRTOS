#ifndef _AVRTOS_KERNEL_H
#define _AVRTOS_KERNEL_H

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

//
// Kernel Public API
//

/**
 * @brief Release the CPU: Stop the execution of the current thread and set it at the end of the runqueue 
 * (if it's still ready) in order to execute it one "cycle" later.
 * This function call the scheduler that determine which thread is the next on to be executing.
 * This functing restore the context of the current thread when returning.
 * This function can be called from either a cooperative thread or a premptive thread
 */
void k_yield(void);

/**
 * @brief Lock the CPU for the current thread being executed. Actually it sets the current 
 * thread as cooperative thread until function k_sched_unlock is called. The syslock is still executed and it stills
 * shift the timed threads in the time queue.
 * As cooperative threads, executing should no last very long, since it could delay other threads execution and break time sensitive (RT) threads.
 */
void k_sched_lock(void);

/**
 * @brief Unlock the CPU for the current thread being executed @see k_sched_lock. Set it as preemptive.
 * 
 * TODO :
 * - for a cooperative thread : yield it
 * - for a preemptive thread, set it as a preemptive thread again
 * - add a new flag that tells if the thread is defined as a preemptive thread but locked the scheduler or is a real cooperative thread
 */
void k_sched_unlock(void);

/**
 * @brief Stop the execution of the current thread for the specified amount of time. This function schedule the execution of
 * the current thread when the timer expires. Returning of this function is equivalent that returning from k_yield.
 * 
 * If timeout is K_FOREVER, the thread will never be executed again, like stopped (TODO review this).
 * 
 * @see k_yield
 * 
 * @param timeout waiting delay
 */
void k_sleep(k_timeout_t timeout);

/*___________________________________________________________________________*/

//
// Not tested
//

/**
 * @brief Stop the execution of the current thread until it is resumed with function k_resume.
 */
void k_suspend(void);

/**
 * @brief Resume suspended thread.
 * 
 * @param th : suspended thread to resume.
 */
void k_resume(struct thread_t *th);

/**
 * @brief Start the thread
 * 
 * @param th : stopped thread to start.
 */
void k_start(struct thread_t *th);

/*___________________________________________________________________________*/

//
// Kernel Private API
//

// for the most of following functions, we assume that the interrupt flag is cleared when called

/**
 * @brief Initialize the runqueue with all threads ready to be executed.
 * Assume that the interrupt flag is cleared when called.
 */
void _k_kernel_init(void);

/**
 * @brief Queue the thread in the runqueue. We assume that the thread {th} is READY. 
 * The thread must not be added to the runqueue already. Keep it's flag unchanged.
 * Assume that the interrupt flag is cleared when called.
 * 
 * @param th : ready thread to queue 
 */
void _k_queue(struct thread_t * const th);

/**
 * @brief Remove the current thread from the runqueue. Keep it's flag unchanged.
 * Assume that the interrupt flag is cleared when called.
 */
void _k_catch(void);

/**
 * @brief Stop the execution of the current thread until it is resumed with function k_resume.
 * State flag is changed to STOPPED.
 * Assume that the interrupt flag is cleared when called.
 */
void _k_suspend(void);

/**
 * @brief Remove the thread from the 
 * Assume that the interrupt flag is cleared when called.
 * 
 * @param th 
 */
void _k_unschedule(struct thread_t *th);

/**
 * @brief Choice the next thread to be executed. 
 * This function is called be any thread switch in order to determine which 
 * one is the following thread to be executed. 
 * 
 * This function is called in k_yield function
 * 
 * @return struct thread_t* : next thread to be executed
 */
/* __attribute__((noinline)) */ struct thread_t *_k_scheduler(void);

/**
 * @brief @see k_yield
 */
void _k_yield(void);

/**
 * @brief Remove the current thread from the runqueue and schedule it's execution later.
 * 
 * @param timeout 
 */
void _k_reschedule(k_timeout_t timeout);

/**
 * @brief Wake up a thread that is waiting for an event.
 * 
 * Assumptions:
 *  - thread is in waiting mode
 *  - thread is not in the runqueue
 *  - thread may be in the events queue
 * 
 * @param th 
 */
void _k_wake_up(struct thread_t *th);

/**
 * @brief Shift time in kernel time queue list (events_queue) 
 */
void _k_system_shift(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 

