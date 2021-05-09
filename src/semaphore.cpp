#include "semaphore.h"

void semaphore_define(sem_t *sem, uint8_t init)
{
    sem->count = init;
}