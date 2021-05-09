#ifndef _MUTEX_H
#define _MUTEX_H

#include <avr/io.h>
#include <stdbool.h>

/*___________________________________________________________________________*/

typedef struct {
    uint8_t lock;

    // TODO, check if the mutex_define caller is the owner of the mutex (for release)
    // thread_t * ower_id
} mutex_t;

/**
 * @brief Define a mutex
 * 
 * @param mutex : structure representing the mutex
 */
void mutex_define(mutex_t *mutex);

extern "C" uint8_t _mutex_read(mutex_t *mutex);

extern "C" uint8_t _mutex_lock(mutex_t *mutex);

/**
 * @brief Lock a mutex
 * 
 * @param mutex : success if 0 else mutex already locked
 * @return uint8_t 
 */
uint8_t mutex_lock(mutex_t *mutex);

/**
 * @brief Release a mutex
 */
extern "C" void mutex_release(mutex_t *mutex);

/*___________________________________________________________________________*/

#endif