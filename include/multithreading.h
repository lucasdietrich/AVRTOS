#ifndef _MULTITHREADING_H
#define _MULTITHREADING_H

#include <avr/io.h>


/*___________________________________________________________________________*/

#define THREAD_2ND_STACK_LOC RAMSTART + (RAMEND - RAMSTART) / 2

/*___________________________________________________________________________*/

typedef struct
{
    void* sp;       // stack pointer (16 bits)
    uint8_t id;     // thread id
} thread_t;

// sizeof(thread_t) == 2

/*___________________________________________________________________________*/

typedef void (*thread_entry_t)(void*);

/**
 * @brief Define a second thread and prepare stack
 * 
 * @param th thread structure pointer
 * @param entry thread function to call
 * @param stack_loc thread stack base location
 * @param p thread context
 */
extern "C" void thread_create(thread_t *th, thread_entry_t entry, uint16_t stack_loc, void* p);

/**
 * @brief TODO
 * 
 */
extern "C" void thread_switch(thread_t *from, thread_t *to);

/*___________________________________________________________________________*/

#endif