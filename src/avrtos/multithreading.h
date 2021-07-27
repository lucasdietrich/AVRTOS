#ifndef _AVRTOS_MULTITHREADING_H
#define _AVRTOS_MULTITHREADING_H

/*___________________________________________________________________________*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <stddef.h>

#include "defines.h"
#include "dstruct/queue.h"
#include "dstruct/dlist.h"
#include "dstruct/tqueue.h"
#include "mutex.h"
#include "semaphore.h"
#include "sysclock.h"

/*___________________________________________________________________________*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This this type represents a thread entry point function
 * 
 * The only parameter (void*) is used to pass the thread context when the entry function is called
 * 
 * This function should never end !
 * 
 * TODO handle the case if the function ends
 */
typedef void (*thread_entry_t)(void*);

enum thread_state_t { STOPPED = 0, RUNNING = 1, READY = 2, WAITING = 3 };

/**
 * @brief This structure represents a thread, it defines:
 * - the value of the stack pointer (valid only when the thread is suspended)
 * - the thread priority (cooperative < 0, preemptive > 0), priority = 0 means the thread is disabled
 * - the stack location (end) and size (size)
 * - the local storage pointer
 * 
 * This structure is 9B long
 * 
 * Warning : Every definition of a "struct thread_t" variable must be stack
 */
struct thread_t
{
    void *sp;       // keep it at the beginning of the structure
    // enum thread_state_t state;
    union {
        struct
        {
            uint8_t state : 2;
            uint8_t coop : 1;
            uint8_t priority : 2;
        };
        uint8_t flags;
    };
    union
    {
        struct ditem runqueue;
        struct titem event;
    } tie;
    struct qitem wmutex;
    struct
    {
        void *end;
        size_t size;
    } stack;
    void *local_storage;
    char symbol;
};


/**
 * @brief This is the thread structure representing the main thread
 * 
 * @see struct thread_t
 */
extern struct thread_t k_thread_main;


extern struct thread_t * k_current;

/*___________________________________________________________________________*/

/**
 * @brief Define a new thread at runtime and initialize its stack
 * 
 * @param th thread structure pointer
 * @param entry thread entry function
 * @param stack thread stack start location
 * @param stack_size thread stack size
 * @param priority thread priority
 * @param context_p thread context passed to entry function
 * @param local_storage thread local_storage
 */
int k_thread_create(struct thread_t *const th, thread_entry_t entry, void *const stack_end, const size_t stack_size, const int8_t priority, void *const context_p, void *const local_storage);

/**
 * @brief Initialize a thread stack at runtime
 * 
 * @param th thread structure pointer
 * @param entry thread entry function
 * @param stack thread stack start location
 * @param context_p thread context
 */
void _k_thread_stack_create(struct thread_t *const th, thread_entry_t entry, void *const stack, void *const context_p);

/*___________________________________________________________________________*/

/**
 * @brief Get current thread
 * 
 * @return thread_t* 
 */
struct thread_t * k_thread_current(void);

/**
 * @brief Get current thread local storage pointer
 * 
 * @return void* local storage pointer
 */
void * k_thread_local_storage(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif