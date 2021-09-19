#ifndef _AVRTOS_MULTITHREADING_H
#define _AVRTOS_MULTITHREADING_H

/*___________________________________________________________________________*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <stddef.h>

#include "dstruct/dlist.h"
#include "dstruct/tqueue.h"

#include "multithreading.h"

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
 * RUNNING : the thread is currently running and can be retrieved with via `_current`, 
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
            uint8_t timer_expired : 1;  // tells if the timer expiration caused this thread to be awakened 
        };
        uint8_t flags;
    };
    union
    {
        struct ditem runqueue;          // represent the thread in the runqueue     (4B)
        struct titem event;             // represent the thread in the events queue (4B)
    } tie;                              // the thread cannot be in the events_queue and the runqueue at the same time
            
    union
    {
        struct ditem wany;              // represent the thread waiting on a generic object
        struct ditem wmutex;            // represent the thread waiting on an mutex
        struct ditem wsem;              // represent the thread waiting on an semaphore
        struct ditem wsig;              // represent the thread waiting on an signal
        struct ditem wfifo;             // represent the thread waiting on a fifo item
    };
    void * swap_data;                   // data returned by kernel API's when calling _k_unpend_first_thread
    struct
    {
        void *end;                      // stack end
        size_t size;                    // stack size
    } stack;                            // thread stack definition
    char symbol;                        // 1-letter symbol to name the thread, already used M (main), idle : I (idle)
};

/**
 * @brief Main thread structure, defined in _k_threads section
 */
extern struct k_thread _k_thread_main;

/**
 * @brief Get the address of the thread currently running.
 */
extern struct k_thread * _current;

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
 * @return int 0 on success
 */
int k_thread_create(struct k_thread* const th, thread_entry_t entry,
    void* const stack, const size_t stack_size, const int8_t priority,
    void* const context_p, const char symbol);

/**
 * @brief Initialize a thread stack at runtime
 * 
 * @param th thread structure pointer
 * @param entry thread entry function
 * @param stack thread stack start location
 * @param context_p thread context
 */
void _k_thread_stack_create(struct k_thread* const th, thread_entry_t entry,
    void* const stack, void* const context_p);

/*___________________________________________________________________________*/

/**
 * @brief Get current thread
 * 
 * @see struct k_thread * _current
 * 
 * @return thread_t* 
 */
struct k_thread * k_thread_current(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif