#ifndef _AVRTOS_MUTEX_H
#define _AVRTOS_MUTEX_H

#include <avr/io.h>
#include <stdbool.h>

#include "avrtos.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Structure representing a mutex, "lock" parameter tells 
 * if the current is locked or not (0 if lock, 0xFF otherwise).
 * 
 * Waitqueue list contains all threads pending for the mutex, 
 * first thread to wake up when mutex is unlocked is first in the queue.
 */
struct k_mutex
{
    uint8_t lock;
    struct ditem waitqueue;
    struct k_thread *owner;
};

/*___________________________________________________________________________*/

#define K_MUTEX_INIT(mutex)                       \
    {                                             \
        .lock = 0xFFu,                            \
        .waitqueue = DLIST_INIT(mutex.waitqueue), \
        .owner = NULL                             \
    }

#define K_MUTEX_DEFINE(mutex_name) \
    static struct k_mutex mutex_name = K_MUTEX_INIT(mutex_name)

/*___________________________________________________________________________*/

/**
 * @brief Initialize a mutex
 * 
 * @param mutex address of the mutex structure
 */
void k_mutex_init(struct k_mutex *mutex);

/**
 * @brief Lock a mutex, return immediately if mutex is available, 
 * otherwise wait until the mutex is unlocked or timeout.
 * 
 * If the mutex is available, the scheduler is minot called. 
 * 
 * If the mutex ic locked, the current thread is unscheduled and 
 * added to the pending queue ("waitqueue") of the mutex, 
 * it will be woke up when the thread reach the top of this 
 * waitqueue and the mutex is available again.
 * 
 * If timeout is different from K_FOREVER, 
 * the thread will be woke up when the timeout expires, in this case
 * the function will check again for the mutex availability. 
 * (current thread is removed from the pending queue).
 * 
 * Don't lock a mutex from an interrupt routine !
 * 
 * If timeout is K_NO_WAIT, this function returns immediately.
 * 
 * @param mutex address of the mutex structure
 * @param timeout time pending the mutex (K_NO_WAIT, K_MSEC(1000), K_FOREVER)
 * @return uint8_t 0 if mutex locked any other value otherwise
 */
K_NOINLINE uint8_t k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout);

/**
 * @brief Unlock a mutex, wake up the first pending thread if the pending queue
 * is not empty.
 * 
 * Switch thread before returning if a thread is pending on the mutex.
 * 
 * This function doesn't check if the current thread 
 * actually own the mutex. This function sets interrupt flag when returning.
 * 
 * Don't unlock a mutex from an interrupt routine !
 * 
 * @param mutex : address of the mutex structure
 */
K_NOINLINE void k_mutex_unlock(struct k_mutex *mutex);

/* concept */
K_NOINLINE uint8_t k_mutex_cancel_wait(struct k_mutex *mutex);

/*___________________________________________________________________________*/

/**
 * @brief Arch lock a mutex, doesn't clear the interrupt flag (if set) 
 * and set it to its original state when finished.
 * 
 * @param mutex address of the mutex structure
 * @return uint8_t 0 if mutex locked any other value otherwise
 */
uint8_t _k_mutex_lock(struct k_mutex *mutex);

/**
 * @brief Arch unlock a mutex, don't need the interrupt flag to be disabled
 * 
 * @param mutex 
 */
void _k_mutex_unlock(struct k_mutex *mutex);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif