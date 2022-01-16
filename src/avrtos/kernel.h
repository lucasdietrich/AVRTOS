#ifndef _AVRTOS_KERNEL_H
#define _AVRTOS_KERNEL_H

#include <avrtos/avrtos.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool __k_interrupts(void);

/*___________________________________________________________________________*/

//
// Kernel Public API
//

#if KERNEL_IRQ_LOCK_COUNTER == 0
/**
 * @brief Disable interrupts in the current thread
 */
#define irq_disable     cli

/**
 * @brief Enable interrupts in the current thread
 */
#define irq_enable      sei

#else

/**
 * @brief Disable interrupts in the current thread.
 * 
 * Can be called recursively.
 */
K_NOINLINE void irq_disable(void);

/**
 * @brief Enable interrupts in the current thread
 * 
 * Can be called recursively.
 */
K_NOINLINE void irq_enable(void);

#endif /* KERNEL_IRQ_LOCK_COUNTER */

/**
 * @brief Release the CPU: Stop the execution of the current thread and set it at the end of the runqueue 
 * (if it's still ready) in order to execute it one "cycle" later.
 * This function call the scheduler that determine which thread is the next on to be executing.
 * This function restore the context of the current thread when returning.
 * This function can be called from either a cooperative thread or a premptive thread.
 */
K_NOINLINE void k_yield(void);

/**
 * @brief Lock the CPU for the current thread being executed. Actually it sets the current 
 * thread as cooperative thread until function k_sched_unlock is called. The syslock is still executed and it stills
 * shift the timed threads in the time queue.
 * As cooperative threads, executing should no last very long, since it could delay other threads execution and break time sensitive (RT) threads.
 * In cooperative threads locking/unlocking the scheduler doesn't have no effect.
 * 
 * Cannot be called from an interrupt routine.
 * 
 * If CONFIG_KERNEL_SCHED_LOCK_COUNTER is defined, k_sched_unlock() can be called recursively. 
 * In this case, the maximum  number of calls without calling k_sched_unlock() is 255.
 * 
 * Note: Scheduler is called anyway if a function yielding the CPU is called 
 *  like k_yield(), k_sleep() or any kernel function waiting for an 
 *  event to be signaled (e.g. k_sem_take(), k_mutex_lock() with delay)
 */
K_NOINLINE void k_sched_lock(void);

/**
 * @brief Unlock the CPU for the current thread being executed @see k_sched_lock. Set it as preemptive.
 * In cooperative threads locking/unlocking the scheduler doesn't have no effect.
 * 
 * Cannot be called from an interrupt routine.
 * 
 * @see k_sched_lock()
 */
K_NOINLINE void k_sched_unlock(void);

/**
 * @brief Tells if the scheduler is locked by the current thread or not.
 * @see k_sched_lock/k_sched_unlock.
 * 
 * @return true     if scheduler is locked for the current thread or thread is cooperative
 * @return false    if not 
 */
K_NOINLINE bool k_sched_locked(void);

/**
 * @brief Tells if current thread is preemptive.
 * 
 * @return K_NOINLINE 
 */
K_NOINLINE bool _k_preemptive(void);

/**
 * @brief Tells if current thread is cooperative
 * 
 * @return K_NOINLINE 
 */
K_NOINLINE bool _k_cooperative(void);

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

/**
 * @brief Make the thread waiting for timeout milliseconds.
 * 
 * Important: Doesn't release the CPU. Poll on k_uptime_get() to know if the timeout has expired.
 * 
 * Note: Can be used in the case there is no IDLE thread.
 * 
 * Note: Require KERNEL_UPTIME to be set.
 * 
 * @param timeout 
 * @return K_NOINLINE 
 */
K_NOINLINE void k_wait(k_timeout_t timeout);

/**
 * @brief Block the RTOS for a specified amount of time.
 * 
 * @param timeout 
 * @return K_NOINLINE 
 */
void k_block(k_timeout_t timeout);

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

/**
 * @brief Stop the thread
 * 
 * @param th : ready/pending thread to start.
 */
K_NOINLINE void _k_stop(void);

/**
 * @brief Stop the thread
 * 
 * @param th : ready/pending thread to start.
 */
K_NOINLINE void k_stop(void);

/*___________________________________________________________________________*/

//
// Kernel Private API
//

// for the most of following functions, we assume that the interrupt flag is cleared when called

/**
 * @brief Do a thread switch (ASM function)
 * 1. save context of the first thread
 * 2. store the SP of the first thread to its structure
 * 3. restore the SP of the second thread from its structure
 * 4. restore context of the second thread
 * 
 * @param from 
 * @param to 
 */
void _k_thread_switch(struct k_thread *from, struct k_thread *to);

/**
 * @brief Initialize the runqueue with all threads ready to be executed.
 * Assume that the interrupt flag is cleared when called.
 */
K_NOINLINE void _k_kernel_init(void);

/**
 * @brief Queue the thread in the runqueue. We assume that the thread {th} is READY. 
 * The thread must not be added to the runqueue already. Keep it's flag unchanged.
 * Assume that the interrupt flag is cleared when called.
 * Assume that the runqueue doesn't contain the IDLE thread
 * 
 * @param th : ready thread to queue 
 */
void _k_queue(struct k_thread *const th);

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
K_NOINLINE void _k_schedule(struct k_thread *thread);

/**
 * @brief Schedule current thread wake up.
 * 
 * Assumptions:
 * - thread is suspended (PENDING)
 * - thread is not in the runqueue
 * 
 * @param thread 
 * @param timeout 
 * @return K_NOINLINE 
 */
K_NOINLINE void _k_schedule_wake_up(k_timeout_t timeout);

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
 * @brief Choose the next thread to be executed. 
 * This function is called during any thread switch in order to determine which 
 * one is the following thread to be executed. 
 * 
 * This function is called in k_yield function
 * 
 * @return struct k_thread* : next thread to be executed
 */
K_NOINLINE void _k_scheduler(void);

K_NOINLINE void _k_scheduler2(void);

static inline void _k_yield(void)
{
	_k_scheduler();
}

static inline void yield(void)
{
	k_yield();
}

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

/**
 * @brief Get uptime in ticks (32 bit), if KERNEL_TICKS is enabled
 * 
 * @return K_NOINLINE 
 */
K_NOINLINE uint32_t k_ticks_get_32(void);

/**
 * @brief Get uptime in ticks (64 bits), if KERNEL_TICKS is enabled
 * 
 * @return K_NOINLINE 
 */
K_NOINLINE uint64_t k_ticks_get_64(void);

/**
 * @brief Get uptime in milliseconds, if KERNEL_UPTIME is enabled
 * 
 * @return K_NOINLINE 
 */
K_NOINLINE uint32_t k_uptime_get_ms32(void);

/**
 * @brief Get uptime in milliseconds, if KERNEL_UPTIME is enabled.
 * 
 * Should be used if KERNEL_UPTIME_40BITS is enabled.
 * 
 * @return K_NOINLINE 
 */
K_NOINLINE uint64_t k_uptime_get_ms64(void);

/**
 * @brief Get uptime in seconds, if KERNEL_UPTIME is enabled
 * 
 * @return K_NOINLINE 
 */
K_NOINLINE uint32_t k_uptime_get(void);

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
K_NOINLINE struct k_thread *_k_unpend_first_thread(struct ditem *waitqueue);

/**
 * @brief Wake up the first thread pending on an object.
 * Set the first pending thread swap_data parameter if set.
 * Switch thread before returning.
 * 
 * @see _k_unpend_first_thread
 * 
 * Assumptions :
 * - interrupt flag is cleared when called
 * - waitqueue is not null
 * - Thread in the runqueue is suspended
 * 
 * @param waitqueue 
 * @param set_swap_data 
 * @return K_NOINLINE struct* 
 */
K_NOINLINE struct k_thread *_k_unpend_first_and_swap(struct ditem *waitqueue,
                                                     void *set_swap_data);

K_NOINLINE void _k_cancel_first_pending(struct ditem *waitqueue);

K_NOINLINE uint8_t _k_cancel_pending(struct ditem *waitqueue);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif 

