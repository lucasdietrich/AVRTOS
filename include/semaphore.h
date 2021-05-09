#ifndef _SEM_H
#define _SEM_H

#include <avr/io.h>
#include <stdbool.h>

/*___________________________________________________________________________*/

typedef struct {
    uint8_t count;

    // TODO, check if the semaphore_define caller is the owner of the semaphore (for release)
    // thread_t * ower_id
} sem_t;

/**
 * @brief Define a mutex
 * 
 * @param mutex : structure representing the mutex
 */
void semaphore_define(sem_t *sem, uint8_t init);

/**
 * @brief Take a semaphore
 * 
 * @param mutex : success if 0 else no semaphore is available
 * @return uint8_t 
 */
extern "C" uint8_t semaphore_take(sem_t *sem);

/**
 * @brief Give a semaphore
 */
extern "C" void semaphore_give(sem_t *sem);

/*___________________________________________________________________________*/

#endif