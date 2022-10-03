/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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


#define CRITICAL_SECTION_BEGIN()    irq_disable()
#define CRITICAL_SECTION_END()      irq_enable()

/**
 * @brief Software reset the microcontroller by calling the reset vector (address 0x0000)
 */
static inline void k_sys_sw_reset(void)
{
	cli();

	/* TODO: jump to reset vector instead of calling it asm("jmp ...")*/
	((void (*) (void)) (0x0000U))();

	__builtin_unreachable();
}

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
K_NOINLINE bool k_cur_is_preempt(void);

/**
 * @brief Tells if current thread is cooperative
 * 
 * @return K_NOINLINE 
 */
K_NOINLINE bool k_cur_is_coop(void);

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
K_NOINLINE void k_thread_start(struct k_thread *th);

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

/**
 * @brief Get the number of threads currently ready. 0 if cpu is IDLE
 * 
 * @return uint8_t 
 */
uint8_t k_ready_count(void);

/* @see k_yield but suppose interrupts are disabled */
void _k_yield(void);

/**
 * @brief Release the CPU: Stop the execution of the current thread and set it at the end of the runqueue 
 * (if it's still ready) in order to execute it one "cycle" later.
 * This function call the scheduler that determine which thread is the next on to be executing.
 * This function restore the context of the current thread when returning.
 * This function can be called from either a cooperative thread or a premptive thread.
 */
static inline void k_yield(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_k_yield();
	}
}

// static inline void k_yield_from_idle()
// {
// 	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
// 		_k_yield_from_idle();
// 	}
// }

void yield(void);

/*___________________________________________________________________________*/

/**
 * @brief Shift time in kernel time queue list (_k_events_queue) 
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

#ifdef __cplusplus
}
#endif

#endif 

