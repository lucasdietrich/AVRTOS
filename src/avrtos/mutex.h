#ifndef _AVRTOS_MUTEX_H
#define _AVRTOS_MUTEX_H

#include <avr/io.h>
#include <stdbool.h>

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#define K_MUTEX_DEFINE(mutex_name) static mutex_t mutex_name = { .lock = 0xFFu, .waitqueue = NULL }

/*___________________________________________________________________________*/

// TODO set all mutex at the same RAM location to iter them easily

typedef struct {
    uint8_t lock;
    struct qitem *waitqueue;
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

/*___________________________________________________________________________*/

uint8_t mutex_lock_wait(mutex_t *mutex, k_timeout_t timeout);

/**
 * @brief Release a mutex
 */
void mutex_release(mutex_t *mutex);

void _mutex_release(mutex_t *mutex);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif