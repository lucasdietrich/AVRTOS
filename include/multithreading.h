#ifndef _MULTITHREADING_H
#define _MULTITHREADING_H

#include <avr/io.h>

#include <stddef.h>

/*___________________________________________________________________________*/

#define THREAD_MAX  4

/*___________________________________________________________________________*/

#define K_PRIO_PREEMPT(p) (p)
#define K_PRIO_COOP(p) (-p)
#define K_PRIO_DEFAULT K_PRIO_COOP(1)
#define K_SOPPED 0

/*___________________________________________________________________________*/

#define THREAD_2ND_STACK_LOC RAMSTART + (RAMEND - RAMSTART) / 2

/*___________________________________________________________________________*/

// Size : 8B
typedef struct
{
    void* sp;       // stack pointer (16 bits)
    uint8_t tid;    // thread id
    int8_t priority; // cooperative < 0, preemptive > 0, 0 : off
    struct {
        uint16_t end;
        size_t size;
    } stack;
} thread_t;

// Size : 3B + 2B*THREAD_MAX
struct k_thread_meta {
    thread_t * current;
    thread_t * list[THREAD_MAX];   // main thread is 0
    uint8_t count;
    uint8_t current_idx;    // todo remove
};

extern thread_t k_thread_main;

extern struct k_thread_meta k_thread;

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
extern "C" void k_thread_create(thread_t *th, thread_entry_t entry, uint16_t stack_loc, void* p);

void k_thread_create(thread_t *th, thread_entry_t entry, uint16_t stack, size_t stack_size, int8_t priority, void* p);

/**
 * @brief TODO
 * 
 */
extern "C" void k_thread_switch(thread_t *from, thread_t *to);

// disable name mangling
extern "C" thread_t *k_schedule(void);


// TODO

extern "C" void k_yield(void);

void cpu_idle(void);

/*___________________________________________________________________________*/

#endif