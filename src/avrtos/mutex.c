#include "mutex.h"

void mutex_define(mutex_t *mutex)
{
    mutex->lock = 0xFFu;
}

uint8_t mutex_lock(mutex_t *mutex)
{
    return _mutex_lock(mutex);
}