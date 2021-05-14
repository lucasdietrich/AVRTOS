#ifndef _MULTITHREADING_H
#define _MULTITHREADING_H

/*___________________________________________________________________________*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <stddef.h>

#include "multithreading_defines.h"

/*___________________________________________________________________________*/

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*thread_entry_t)(void*);

// Size : 9B
// "thread_t" must be static !!
struct thread_t
{
    void* sp;       // stack pointer (16 bits)
    int8_t priority; // cooperative < 0, preemptive > 0, 0 : off
    struct {
        void * end;
        size_t size;
    } stack;
    void * local_storage;
};

// Size : 3B + 2B*THREAD_MAX
// duplication : current and current_idx does represent the same thing
// todo remove current and shift in multithreading.asm
struct k_thread_meta {
    struct thread_t *current;                            // used in multithreading.asm (when saving current thread context)
    struct thread_t * list[K_THREAD_MAX_COUNT];   // main thread is 0
    uint8_t count;
    uint8_t current_idx;                          // current index
};

extern struct thread_t k_thread_main;

extern struct k_thread_meta k_thread;

// static char _k_main_stack[THREAD_MAIN_STACK_SIZE];

/*___________________________________________________________________________*/

/**
 * @brief Define a second thread and prepare stack
 * 
 * @param th thread structure pointer
 * @param entry thread function to call
 * @param stack thread stack start location
 * @param p thread context
 */

void k_thread_stack_create(struct thread_t *const th, thread_entry_t entry, void *const stack, void *const context_p);

void k_thread_create(struct thread_t *const th, thread_entry_t entry, void *const stack_end, const size_t stack_size, const int8_t priority, void *const context_p, void *const local_storage);

int k_thread_register(struct thread_t *const th);

/*___________________________________________________________________________*/

/**
 * @brief switch from current thrad *from to *to
 */
void _k_thread_switch(struct thread_t *from, struct thread_t *to);

struct thread_t *k_scheduler(void);

/**
 * @brief Release CPU
 */
void k_yield(void);

// TODO
void cpu_idle(void);

/*___________________________________________________________________________*/

/**
 * @brief Get current thread local storage pointer
 * 
 * @return void* 
 */
void * k_thread_local_storage(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif
#endif