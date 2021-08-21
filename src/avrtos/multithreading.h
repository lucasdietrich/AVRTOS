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
#include "workqueue.h"
#include "semaphore.h"
#include "sysclock.h"
#include "idle.h"
#include "canaries.h"

/*___________________________________________________________________________*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This this type represents a thread entry point function
 * 
 * The only parameter (void*) is used to pass the thread context when the entry function is called.
 * 
 * This function should never end (TODO handle the case when the function ends)
 */
typedef void (*thread_entry_t)(void*);

/**
 * @brief Thread state.
 * 
 * STOPPED : the thread is not running and is not in the runqueue, it can be resumed/started with k_resume/k_start functions.
 * 
 * RUNNING : the thread is currently running and can be retrieved with via `k_current`, 
 * moreover the thread is at the top of the 'runqueue'.
 * 
 * READY : the thread is ready to be executed and it somewhere is in the runqueue but is not at the top
 * 
 * WAITING : the thread is waiting for an event, it may be in the time queue (events_queue) but it is not in the runqueue.
 * It can be wake up with function _k_wake_up()
 * 
 */
enum thread_state_t { STOPPED = 0, RUNNING = 1, READY = 2, WAITING = 3 };

/**
 * @brief This structure represents a thread, it defines:
 * - the value of the stack pointer (valid only when the thread is suspended)
 * - the thread priority (cooperative < 0, preemptive > 0), priority = 0 means the thread is disabled
 * - the stack location (end) and size (size)
 * - the local storage pointer
 * 
 * This structure is 16B long
 * 
 * Warning : Every definition of a "struct k_thread" variable must be stack
 */
struct k_thread
{
    void *sp;       // stack point, keep it at the beginning of the structure
    union {
        struct
        {
            uint8_t state : 2;          // @see thread_state_t
            uint8_t coop : 1;           // cooperative/preemptive thread
            uint8_t priority : 2;       // thread priority : not supported for now
            uint8_t immediate : 1;      // tell that the thread is the next to be executed 
                                        // and should not be reordered (by the scheduler for example)
                                        // this flag is not persistent and will be cleared when the thread is executed
                                        // TODO, maybe remove this immediate flag and push event ready thread after the normally next thread
        };
        uint8_t flags;
    };
    union
    {
        struct ditem runqueue;          // represent the thread in the runqueue     (4B)
        struct titem event;             // represent the thread in the events queue (4B)
    } tie;                              // the thread cannot be in the events_queue and the runqueue at the same time
                                        
    struct qitem wmutex;                // represent the thread waiting on an object (mutex, signal, ...)
    struct
    {
        void *end;                      // stack end
        size_t size;                    // stack size
    } stack;                            // thread stack definition
    void *local_storage;                // pointer to a custom user object, local storage of the thread
                                        // we cannot use CONTAINER_OF(thread, struct mystruct, thread_struct), 
                                        // because all threads structures are stack at the same address location in RAM
    char symbol;                        // symbol association to the thread, already used main : M, idle : K, ...
};

/**
 * @brief Main thread structure, defined in _k_threads section
 */
extern struct k_thread _k_thread_main;

/**
 * @brief Get the address of the thread currently running.
 */
extern struct k_thread * k_current;

/**
 * @brief Get the number of threads defined.
 */
extern uint8_t _k_thread_count;

/*___________________________________________________________________________*/

/**
 * @brief 
 * 
 * @param th thread structure pointer
 * @param entry thread entry function
 * @param stack thread stack start location
 * @param stack_size thread stack size
 * @param priority thread priority
 * @param context_p thread context passed to entry function
 * @param local_storage thread local_storage
 */

/**
 * @brief Define a new thread at runtime and initialize its stack
 * 
 * @param th hread structure pointer
 * @param entry thread entry function
 * @param stack_end thread stack start location
 * @param stack_size thread stack size
 * @param priority thread priority
 * @param context_p thread context passed to entry function
 * @param local_storage thread local_storage
 * @return int 0 on success
 */
int k_thread_create(struct k_thread *const th, thread_entry_t entry, void *const stack_end, const size_t stack_size, const int8_t priority, void *const context_p, void *const local_storage);

/**
 * @brief Initialize a thread stack at runtime
 * 
 * @param th thread structure pointer
 * @param entry thread entry function
 * @param stack thread stack start location
 * @param context_p thread context
 */
void _k_thread_stack_create(struct k_thread *const th, thread_entry_t entry, void *const stack, void *const context_p);

/*___________________________________________________________________________*/

/**
 * @brief Get current thread
 * 
 * @see struct k_thread * k_current
 * 
 * @return thread_t* 
 */
struct k_thread * k_thread_current(void);

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