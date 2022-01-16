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
 * READY : the thread is ready to be executed and is the runqueue
 * 
 * PENDING : the thread is pending for an event, it may be in the time queue (events_queue) but it is not in the runqueue.
 * It can be wake up with function _k_wake_up()
 * 
 */
enum thread_state_t { STOPPED = 0, READY = 1, PENDING = 2, _UNDEFINED = 3};

/* size 19B */
struct _k_callsaved_ctx {
	uint8_t sreg;
	
	uint8_t r29;
	uint8_t r28;

	uint8_t r17;
	uint8_t r16;
	uint8_t r15;
	uint8_t r14;
	uint8_t r13;
	uint8_t r12;
	uint8_t r11;
	uint8_t r10;
	uint8_t r9;
	uint8_t r8;
	uint8_t r7;
	uint8_t r6;
	union {
		struct {
			uint8_t r5;
			uint8_t r4;
		};
		void *thread_entry;
	};
	union {
		struct {
			uint8_t r3;
			uint8_t r2;
		};
		void *thread_context;
	};

	/* what about r0 */

	/* DOCUMENTATION
	 * R0, T-Flag: 
	 * The temporary register and the T-flag in SREG are also call-clobbered, but this knowledge is not exposed explicitly to the compiler (R0 is a fixed register).
	 */
};

struct _k_thread_context_ext
{
	uint8_t r18;
	uint8_t r19;
	uint8_t r20;
	uint8_t r21;
	uint8_t r22;
	uint8_t r23;
	uint8_t r24;
	uint8_t r25;
	uint8_t r26;
	uint8_t r27;
	uint8_t r30;
	uint8_t r31;
};

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
			/* @see thread_state_t */
                        uint8_t state : 2;

			/* tells if scheduler is temporarely locked */
                        uint8_t sched_lock : 1;

			/* cooperative/preemptive thread */
                        uint8_t coop : 1;

			/* thread priority : not supported for now */
                        uint8_t priority : 1;

			/* tells if the timer expiration caused 
			 * this thread to be awakened 
			 */
                        uint8_t timer_expired : 1;

			/* tells if the thread pending on 
			 * an object was canceled
			 */
                        uint8_t pend_canceled : 1;
			
			/* tells if the thread wake up is scheduled 
			 * in events_queue (flag for optimisation purpose)
			 */
			uint8_t wakeup_schd: 1;      
                };
                uint8_t flags;
        };
        union
        {
                struct ditem runqueue;          // represent the thread in the runqueue     (4B)
                struct titem event;             // represent the thread in the events queue (4B)
        } tie;                                  // the thread cannot be in the events_queue and the runqueue at the same time

        union
        {
                struct ditem wany;              // represent the thread pending on a generic object
                struct ditem wmutex;            // represent the thread pending on an mutex
                struct ditem wsem;              // represent the thread pending on an semaphore
                struct ditem wsig;              // represent the thread pending on an signal
                struct ditem wfifo;             // represent the thread pending on a fifo item
                struct ditem wmsgq;             // represent the thread pending on a msgq item
        };
        void *swap_data;                        // data returned by kernel API's when calling _k_unpend_first_thread

#if KERNEL_IRQ_LOCK_COUNTER
        /**
         * @brief Depth of calls to irq_disable()
         */
        uint8_t irq_lock_cnt;
#endif /* KERNEL_IRQ_LOCK_COUNTER */

#if KERNEL_SCHED_LOCK_COUNTER
        /**
         * @brief Depth of calls to k_sched_lock()
         */
        uint8_t sched_lock_cnt;
#endif /* KERNEL_SCHED_LOCK_COUNTER */

        struct
        {
                void *end;                      // stack end
                size_t size;                    // stack size
        } stack;                                // thread stack definition
        char symbol;                            // 1-letter symbol to name the thread, already used M (main), idle : I (idle)

#if THREAD_ERRNO
        uint8_t errno;                          // Thread errno
#endif /* THREAD_ERRNO */

#if KERNEL_THREAD_MONITORING
	uint32_t ticks; /* TODO implement uint40 */
#endif /* KERNEL_THREAD_MONITORING */
};

/**
 * @brief Main thread structure, defined in _k_threads section
 */
extern struct k_thread _k_thread_main;

/**
 * @brief Get the address of the thread currently running.
 */
extern struct k_thread * _current;

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
 * @param stack thread stack start location
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

void _k_thread_entry(void);

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