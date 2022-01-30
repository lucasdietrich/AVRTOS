#ifndef _AVRTOS_SEM_H
#define _AVRTOS_SEM_H

#include <avr/io.h>
#include <stdbool.h>

#include "avrtos.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Structure describing a semaphore, "count" parameter tells the number 
 * of available semaphore, "limit" describe the maximum number of semaphores.
 * 
 * Waitqueue list contains all threads pending for the semaphore, 
 * first thread to wake up when a semaphore is available is at the 
 * first element of the queue.
 */
struct k_sem
{
    uint8_t count;
    uint8_t limit;
    struct ditem waitqueue;
};

/*___________________________________________________________________________*/

#define K_SEM_INIT(sem, initial_count, count_limit) \
    {                                               \
        .count = MIN(initial_count, count_limit),   \
        .limit = count_limit,                       \
        .waitqueue = DLIST_INIT(sem.waitqueue)      \
    }

#define K_SEM_DEFINE(sem_name, initial_count, count_limit) \
    struct k_sem sem_name = K_SEM_INIT(sem_name, initial_count, count_limit)

/*___________________________________________________________________________*/

/**
 * @brief Initialize a semaphore
 * 
 * @param sem : structure representing the semaphore
 */
void k_sem_init(struct k_sem *sem, uint8_t initial_count, uint8_t limit);

/**
 * @brief Take a semaphore, return immediately if a semaphore is available, 
 * otherwise wait until a semaphore is given or timeout.
 * 
 * If a semaphore is available, the scheduler is not called. 
 * 
 * If no semaphore is available, the current thread is unscheduled and added 
 * to the pending queue ("waitqueue") of the semaphore,  it will be woke up 
 * when the thread reach the top of this waitqueue and a semaphore is available.
 * 
 * If timeout is different from K_FOREVER, the thread will be woke up 
 * when the timeout expires, in this case the function will check
 * again for a semaphore availability. 
 * (current thread is removed from the pending queue).
 * 
 * Cannot be called from an interrupt routine 
 * if timeout is different from K_NO_WAIT.
 * 
 * @param sem address of the semaphore structure
 * @param timeout time waiting the semaphore (e.g. K_NO_WAIT, K_MSEC(1000), K_FOREVER)
 * @return uint8_t 0 if a semaphore is taken any other value otherwise
 */
K_NOINLINE int8_t k_sem_take(struct k_sem *sem, k_timeout_t timeout);

/**
 * @brief Give a semaphore, wake up the first pending thread if the pending 
 * queue is not empty.
 * 
 * Switch thread before returning if a thread is pending on a semaphore.
 * 
 * This function sets interrupt flag when returning.
 * 
 * Give a semaphore if the "limit" is reached will have no effect.
 * 
 * Can be called from an interrupt routine.
 * 
 * @param sem : address of the semaphore structure
 */
K_NOINLINE void k_sem_give(struct k_sem *sem);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif