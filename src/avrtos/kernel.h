#ifndef _AVRTOS_KERNEL_H
#define _AVRTOS_KERNEL_H

#include "avrtos.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool __k_interrupts(void);

/*___________________________________________________________________________*/

//
// Kernel Public API
//

/**
 * @brief Enable interrupts in the current thread
 */
#define irq_enable      sei

/**
 * @brief Disable interrupts in the current thread
 */
#define irq_disable     cli

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
 * In cooperative threads locking/unlocking the scheduler doesn't have no effect.
 */
K_NOINLINE void k_sched_lock(void);

/**
 * @brief Unlock the CPU for the current thread being executed @see k_sched_lock. Set it as preemptive.
 * In cooperative threads locking/unlocking the scheduler doesn't have no effect.
 * TODO :
 * - for a cooperative thread : yield it
 * - for a preemptive thread, set it as a preemptive thread again
 * - add a new flag that tells if the thread is defined as a preemptive thread but locked the scheduler or is a real cooperative thread
 * 
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
 * @brief used in K_SCHED_LOCK_CONTEXT macro
 * 
 * @see k_sched_unlock
 * 
 * @param __s 
 * @return __inline__ 
 */
static __inline__ void __k_sched_restore(const uint8_t *__s)
{
        k_sched_unlock();
        __asm__ volatile ("" ::: "memory");
        ARG_UNUSED(__s);
}

/**
 * @brief used in K_SCHED_LOCK_CONTEXT macro
 * 
 * @return __inline__ 
 */
static __inline__ uint8_t __k_sched_lock_ret(void)
{
        k_sched_lock();

        return 1;
}

/**
 * @brief This macro lock the scheduler for the duration of the inner scope.
 * 
 * This code in the block is equivalent to the code between k_sched_lock(); 
 *    and k_sched_unlock();
 * 
 * Inspiration for this MACRO come greatly from 
 *    ATOMIC_BLOCK(ATOMIC_FORCEON) { } macro from <avr/atomic.h> 
 */
#define K_SCHED_LOCK_CONTEXT                                                       \
   for (uint8_t __k_schedl_x __attribute__((__cleanup__(__k_sched_restore))) = 0u, \
                             __k_todo = __k_sched_lock_ret();                      \
        __k_todo;                                                                  \
        __k_todo = 0)
 
/**
 * @brief Stop the execution of the current thread for the specified amount of time. This function schedule the execution of
 * the current thread when the timer expires. Returning of this function is equivalent that returning from k_yield.
 * 
 * If timeout is K_FOREVER, the thread will never be executed again, like stopped (TODO review this).
 * 
 * @see k_yield
 * 
 * @param timeout pending timeout
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
K_NOINLINE void _k_schedule(struct ditem *const thread_tie);

/**
 * @brief Schedule the thread wake up.
 * 
 * Assumptions:
 * - thread is suspended (PENDING)
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
 * State flag is changed to PENDING.
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
 * @brief Wake up a thread that is pending for an event.
 * 
 * Assumptions:
 *  - thread is in PENDING mode
 *  - thread is not in the runqueue
 *  - thread may be in the events queue
 *  - interrupt flag is cleared when called.
 *
 * 
 * @param th thread to wake up
 */
K_NOINLINE void _k_wake_up(struct k_thread *th);

/**
 * @brief Suspend the current thread and schedule its awakening for later
 * 
 * Assumptions :
 *  - interrupt flag is cleared when called.
 * 
 * @param timeout 
 */
K_NOINLINE void _k_reschedule(k_timeout_t timeout);

/*___________________________________________________________________________*/

/**
 * @brief Shift time in kernel time queue list (events_queue) 
 * and process timers if any
 * 
 * Assumptions :
 *  - interrupt flag is cleared when called.
 */
K_NOINLINE void _k_system_shift(void);

/*___________________________________________________________________________*/

/**
 * @brief Make the current thread waiting/pending for an object being available.
 * 
 * Suspend the thread and add it to the waitqueue.
 * The function will return if the thread is awakaned or on timeout.
 * 
 * If timeout is K_FOREVER, the thread should we awakaned.
 * If timeout is K_NO_WAIT, the thread returns immediately
 * 
 * Assumptions :
 *  - interrupt flag is cleared when called.
 * 
 * @param waitqueue 
 * @param timeout 
 * @return 0 on success (object available), ETIMEOUT on timeout, negative error
 *  in other cases.
 */
K_NOINLINE int8_t _k_pend_current(struct ditem *waitqueue, k_timeout_t timeout);

/**
 * @brief Wake up the first thread pending on an object.
 * Set the first pending thread swap_data parameter before switching.
 * Switch thread before returning.
 * 
 * Assumptions :
 * - interrupt flag is cleared when called
 * - waitqueue is not null
 * - Thread in the runqueue is suspended
 * 
 * @param waitqueue 
 * @param swap_data : available object information
 * @return uint8_t return 0 if a thread got the object, any other value otherwise
 */
K_NOINLINE uint8_t _k_unpend_first_thread(struct ditem *waitqueue, void * swap_data);

/*___________________________________________________________________________*/

K_NOINLINE void _k_on_thread_return(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 

