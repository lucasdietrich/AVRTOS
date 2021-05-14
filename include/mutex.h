#ifndef _MUTEX_H
#define _MUTEX_H

#include <avr/io.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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

uint8_t _mutex_lock(mutex_t *mutex);

/**
 * @brief Lock a mutex
 * 
 * @param mutex : success if 0 else mutex already locked
 * @return uint8_t 
 */

// TODO set this inline
// https://stackoverflow.com/questions/5057021/why-are-c-inline-functions-in-the-header
uint8_t mutex_lock(mutex_t *mutex);

/**
 * @brief Release a mutex
 */
void mutex_release(mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif