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
 * This function restore the context of the current thread when returning.
 * This function can be called from either a cooperative thread or a premptive thread.
 */
void k_yield(void);

/**
 * @brief Lock the CPU for the current thread being executed. Actually it sets the current 
 * thread as cooperative thread until function k_sched_unlock is called. The syslock is still executed and it stills
 * shift the timed threads in the time queue.
 * As cooperative threads, executing should no last very long, since it could delay other threads execution and break time sensitive (RT) threads.
 */
K_NOINLINE void k_sched_lock(void);

/**
 * @brief Unlock the CPU for the current thread being executed @see k_sched_lock. Set it as preemptive.
 * 
 * TODO :
 * - for a cooperative thread : yield it
 * - for a preemptive thread, set it as a preemptive thread again
 * - add a new flag that tells if the thread is defined as a preemptive thread but locked the scheduler or is a real cooperative thread
 */
K_NOINLINE void k_sched_unlock(void);

/**
 * @brief Tells if the scheduler is locked by the current thread or not.
 * @see k_sched_lock/k_sched_unlock.
 * 
 * @return true     if scheduler is locked for the current thread or is thread is cooperative
 * @return false    if not 
 */
K_NOINLINE bool k_sched_locked(void);

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
K_NOINLINE void k_sleep(k_timeout_t timeout);

/*___________________________________________________________________________*/

//
// Not tested
//

/**
 * @brief Stop the execution of the current thread until it is resumed with function k_resume.
 */
K_NOINLINE void k_suspend(void);

/**
 * @brief Resume suspended thread.
 * 
 * @param th : suspended thread to resume.
 */
K_NOINLINE void k_resume(struct k_thread *th);

/**
 * @brief Start the thread
 * 
 * @param th : stopped thread to start.
 */
K_NOINLINE void k_start(struct k_thread *th);

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
 * Assume that the runqueue doesn't contain the IDLE thread
 * 
 * @param th : ready thread to queue 
 */
void _k_queue(struct k_thread * const th);

/**
 * @brief Schedule the thread to be executed.
 * If the IDLE thread is in the runqueue (it is removed), the scheduled thread become the only thread in the runqueue.
 * If other threads are in the runqueue, the thread is only appended.
 * - Assume that the thread is READY
 * - Assume that the thread is not in the runqueue
 * 
 * @param thread_tie thread.tie.runqueue item
 * @return __attribute__((noinline)) 
 */
K_NOINLINE void _k_schedule(struct ditem * const thread_tie);

/**
 * @brief Schedule the thread wake up.
 * 
 * Assumptions:
 * - thread is suspended (WAITING)
 * - thread is not in the runqueue
 * 
 * @param thread 
 * @param timeout 
 * @return K_NOINLINE 
 */
K_NOINLINE void _k_schedule_wake_up(struct k_thread *thread, k_timeout_t timeout);

/**
 * @brief Remove the current thread from the runqueue.
 * Stop the execution of the current thread (until it is scheduled again with function _k_schedule or _k_schedule_wake_up)
 * State flag is changed to WAITING.
 * 
 * Assumptions:
 * - interrupt flag is cleared when called.
 * - thread is in the runqueue
 */
K_NOINLINE void _k_suspend(void);

/**
 * @brief Remove the thread from the events queue
 * 
 * Assumptions:
 * - interrupt flag is cleared when called.
 * - thread is in the time_queue
 * 
 * @param th 
 */
K_NOINLINE void _k_unschedule(struct k_thread *th);

/**
 * @brief Choose the next thread to be executed. 
 * This function is called during any thread switch in order to determine which 
 * one is the following thread to be executed. 
 * 
 * This function is called in k_yield function
 * 
 * @return struct k_thread* : next thread to be executed
 */
K_NOINLINE struct k_thread *_k_scheduler(void);

/**
 * @brief Wake up a thread that is waiting for an event.
 * 
 * Assumptions:
 *  - thread is in waiting mode
 *  - thread is not in the runqueue
 *  - thread may be in the events queue
 *  - interrupt flag is cleared when called.
 *
 * 
 * @param th thread to wake up
 */
K_NOINLINE void _k_wake_up(struct k_thread *th);

/**
 * @brief Wake up a thread that is waiting for an event.
 * This thread is the very first to be executed.
 * The scheduler should not reorder it before beeing executed.
 * 
* Assumptions:
 *  - thread is in waiting mode
 *  - thread is not in the runqueue
 *  - thread may be in the events queue
 *  - interrupt flag is cleared when called.
 * 
 * @param th thread to wake up in immediate mode
 */
K_NOINLINE void _k_immediate_wake_up(struct k_thread *th);

/**
 * @brief Suspend the current thread and schedule its awakening for later
 * 
 * @param timeout 
 */
K_NOINLINE void _k_reschedule(k_timeout_t timeout);

/*___________________________________________________________________________*/

/**
 * @brief Shift time in kernel time queue list (events_queue) 
 */
K_NOINLINE void _k_system_shift(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 

