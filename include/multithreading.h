#ifndef _MULTITHREADING_H
#define _MULTITHREADING_H

#include <avr/io.h>

/*___________________________________________________________________________*/

typedef struct
{
    void* sp;       // stack pointer (16 bits)
    void* data;     // pointer to context (16 bits)
} thread_t;

/*___________________________________________________________________________*/

extern "C" void thread_switch(thread_t *th_from, thread_t *th_to);

/*___________________________________________________________________________*/



#endif