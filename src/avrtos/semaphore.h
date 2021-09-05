#ifndef _AVRTOS_SEM_H
#define _AVRTOS_SEM_H

#include <avr/io.h>
#include <stdbool.h>

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#define K_SEM_INIT(initial_count, count_limit)    \
    {                                             \
        .count = MIN(initial_count, count_limit), \
        .limit = count_limit,                     \
        .waitqueue = NULL                         \
    }

#define K_SEM_DEFINE(sem_name, initial_count, count_limit) \
    static struct k_sem sem_name = K_SEM_INIT(initial_count, count_limit)

/*___________________________________________________________________________*/

/**
 * @brief Structure describing a semaphore, "count" parameter tells the number 
 * of available semaphore, "limit" describe the maximum number of semaphores.
 * 
 * Waitqueue list contains all threads waiting for the semaphore, 
 * first thread to wake up when a semaphore is available is at the 
 * first element of the queue.
 */
struct k_sem
{
    uint8_t count;
    uint8_t limit;
    struct ditem *waitqueue;
};

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
 * to the waiting queue ("waitqueue") of the semaphore,  it will be woke up 
 * when the thread reach the top of this waitqueue and a semaphore is available.
 * 
 * If timeout is different from K_FOREVER, the thread will be woke up 
 * when the timeout expires, in this case the function will check
 * again for a semaphore availability. 
 * (current thread is removed from the waiting queue).
 * 
 * Don't take a semaphore from an interrupt routine !
 * 
 * @param sem address of the semaphore structure
 * @param timeout time waiting the semaphore (e.g. K_NO_WAIT, K_MSEC(1000), K_FOREVER)
 * @return uint8_t 0 if a semaphore is taken any other value otherwise
 */
K_NOINLINE uint8_t k_sem_take(struct k_sem *sem, k_timeout_t timeout);

/**
 * @brief Give a semaphore, wake up the first waiting thread if the waiting 
 * queue is not empty, do k_yield.
 * 
 * This function sets interrupt flag when returning.
 * 
 * Give a semaphore if the "limit" is reached will have no effect.
 * 
 * @param sem : address of the semaphore structure
 */
K_NOINLINE void k_sem_give(struct k_sem *sem);

/*___________________________________________________________________________*/

/**
 * @brief Arch take a semaphore, doesn't clear the interrupt flag.
 * 
 * @param sem address of the semaphore structure
 * @return uint8_t 0 if a semaphore is taken any other value otherwise
 */
uint8_t _k_sem_take(struct k_sem *sem);

/**
 * @brief Arch give a semaphore, doesn't the interrupt flag (if set).
 * 
 * Giving a semaphore if the "limit" is reached will have no effect.
 * 
 * Can be called from an interrupt.
 * 
 * @param sem 
 */
void _k_sem_give(struct k_sem *sem);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif