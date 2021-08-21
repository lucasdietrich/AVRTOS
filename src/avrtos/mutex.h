#ifndef _AVRTOS_MUTEX_H
#define _AVRTOS_MUTEX_H

#include <avr/io.h>
#include <stdbool.h>

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#define K_MUTEX_INIT() { .lock = 0xFFu, .waitqueue = NULL }
#define K_MUTEX_DEFINE(mutex_name) static struct k_mutex mutex_name = { .lock = 0xFFu, .waitqueue = NULL }

/*___________________________________________________________________________*/

/**
 * @brief Structure describing a mutex, "lock" parameter tells if the current is locked or not (0 if lock, 0xFF otherwise)
 * waitqueue list contains all threads waiting for the mutex, first thread to wake up when mutex is release is at the first element of the queue
 */
struct k_mutex {
    uint8_t lock;
    struct qitem *waitqueue;
};

/*___________________________________________________________________________*/

/**
 * @brief Initialize a mutex
 * 
 * @param mutex address of the mutex structure
 */
void k_mutex_init(struct k_mutex *mutex);

/**
 * @brief Lock a mutex, return immediately (nonblocking @see k_mutex_lock_wait).
 * 
 * @param mutex address of the mutex structure
 * @return uint8_t 0 if mutex locked any other value otherwise
 */
uint8_t k_mutex_lock(struct k_mutex *mutex);

/**
 * @brief Lock a mutex, return immediately if mutex is available, otherwise wait until the mutex is released or timeout.
 * If the mutex is available, the scheduler is not called. If the mutex ic locked,
 * the current thread is unscheduled and added to the waiting queue ("waitqueue") of the mutex, 
 * it will be woke up when the thread reach the top of this waitqueue and the mutex is available again.
 * If timeout is different from K_FOREVER, the thread will be woke up when the timeout expires, in this case
 * the function will check again for the mutex availability. (current thread is removed from the waiting queue).
 * Don't lock a mutex from an interrupt routine !
 * 
 * @param mutex address of the mutex structure
 * @param timeout time waiting the mutex (e.g. K_NO_WAIT, K_MSEC(1000), K_FOREVER)
 * @return uint8_t 0 if mutex locked any other value otherwise
 */
uint8_t k_mutex_lock_wait(struct k_mutex *mutex, k_timeout_t timeout);

/**
 * @brief Release a mutex, wake up the first waiting thread if the waiting queue is not empty, do k_yield
 * this function doesn't check if the current thread actually own the mutex. This function sets interrupt flag when returning.
 * Can be called from an interrupt.
 * 
 * @param mutex : address of the mutex structure
 */
void k_mutex_release(struct k_mutex *mutex);

/*___________________________________________________________________________*/

/**
 * @brief Arch lock a mutex, clear the interrupt flag (if set) and set it to its original state when finished
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
void _k_mutex_release(struct k_mutex *mutex);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif