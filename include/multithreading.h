#ifndef _MULTITHREADING_H
#define _MULTITHREADING_H

#include <avr/io.h>

/*___________________________________________________________________________*/

typedef struct
{
    void* sp;       // stack pointer (16 bits)

    void* data;     // pointer to context (16 bits)
} thread_t;

// sizeof(thread_t) == 4

/*___________________________________________________________________________*/

extern "C" void thread_switch(thread_t *from, thread_t *to);

extern "C" void thread_define(thread_t *th, uint16_t stack);

/*___________________________________________________________________________*/



#endif