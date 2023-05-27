/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_MULTITHREADING_H
#define _AVRTOS_MULTITHREADING_H

#include "defines.h"
#include "dstruct/dlist.h"
#include "dstruct/tqueue.h"

#include <stddef.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This this type represents a thread entry point function
 *
 * The only parameter (void*) is used to pass the thread context when the entry
 * function is called.
 */
typedef void (*thread_entry_t)(void *);

/* size 19B */
struct z_callsaved_ctx {
	uint8_t sreg;

	union {
		uint8_t regs[13];
		struct {
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
		};
	};
	union {
		uint8_t r6;
		uint8_t init_sreg;
	};
	union {
		struct {
			uint8_t r5;
			uint8_t r4;
		};
		thread_entry_t thread_entry;
	};
	union {
		struct {
			uint8_t r3;
			uint8_t r2;
		};
		void *thread_context;
	};

	/*
	 * DOCUMENTATION
	 * R0, T-Flag:
	 * The temporary register and the T-flag in SREG are also
	 * call-clobbered, but this knowledge is not exposed explicitly to the
	 * compiler (R0 is a fixed register).
	 */

	struct {
#if Z_ARCH_PC_SIZE == 3
		uint8_t pch;
#endif /* Z_ARCH_PC_SIZE == 3 */

		void *pc;
	};
};

struct z_callused_ctx {
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

struct z_intctx {
#if Z_ARCH_PC_SIZE == 3
	uint8_t pch;
#endif /* Z_ARCH_PC_SIZE == 3 */
	void *pc;

	uint8_t r1;
	uint8_t r0;
	uint8_t sreg;

	struct z_callused_ctx callused_reg;
};

/**
 * @brief This structure represents a thread, it defines:
 * - the value of the stack pointer (valid only when the thread is suspended)
 * - the thread priority (cooperative < 0, preemptive > 0), priority = 0 means
 * the thread is disabled
 * - the stack location (end) and size (size)
 * - the local storage pointer
 *
 * This structure is 16B long
 *
 * Warning : Every definition of a "struct k_thread" variable must be stack
 */
struct k_thread {
	void *sp;  // stack point, keep it at the beginning of the structure

	uint8_t flags;	// thread flags

	union {
		struct dnode runqueue;	// represent the thread in the runqueue (4B)
		struct titem event;	// represent the thread in the events queue (4B)
	} tie;	// the thread cannot be in the events_queue and the runqueue at
		// the same time

	union {
		struct dnode wany;    // represent the thread pending on a generic
				      // object
		struct dnode wmutex;  // represent the thread pending on an mutex
		struct dnode wsem;    // represent the thread pending on an semaphore
		struct dnode wsig;    // represent the thread pending on an signal
		struct dnode wfifo;   // represent the thread pending on a fifo item
		struct dnode wmsgq;   // represent the thread pending on a msgq item
		struct dnode wflags;  // represent the thread pending on a flags item
	};
	void *swap_data;  // data returned by kernel API's when calling
			  // z_unpend_first_thread

#if CONFIG_KERNEL_IRQ_LOCK_COUNTER
	/**
	 * @brief Depth of calls to irq_disable()
	 */
	uint8_t irq_lock_cnt;
#endif /* CONFIG_KERNEL_IRQ_LOCK_COUNTER */

#if CONFIG_KERNEL_SCHED_LOCK_COUNTER
	/**
	 * @brief Depth of calls to k_sched_lock()
	 */
	uint8_t sched_lock_cnt;
#endif /* CONFIG_KERNEL_SCHED_LOCK_COUNTER */

	struct {
		void *end;    // stack end
		size_t size;  // stack size
	} stack;	      // thread stack definition
	char symbol;	      // 1-letter symbol to name the thread, reserver M (main),
			      // idle : I (idle)

#if CONFIG_THREAD_ERRNO
	uint8_t errno;	// Thread errno
#endif			/* CONFIG_THREAD_ERRNO */
};

/**
 * @brief Define a new thread at runtime and initialize its stack
 *
 * @param thread hread structure pointer
 * @param entry thread entry function
 * @param stack thread stack start location
 * @param stack_size thread stack size
 * @param priority thread priority
 * @param context_p thread context passed to entry function
 * @param symbol thread symbol letter
 * @return int 0 on success
 */
int8_t k_thread_create(struct k_thread *thread,
		       thread_entry_t entry,
		       void *stack,
		       size_t stack_size,
		       uint8_t priority,
		       void *context_p,
		       char symbol);

/**
 * @brief Thread entry point function.
 *
 * This function serves as the entry point for a new thread. It is responsible for
 * executing the code within the thread and receives a context pointer as an argument.
 *
 * @param context A pointer to the context data for the thread.
 */
void z_thread_entry(void *context);

extern struct k_thread *z_current;

/**
 * @brief Get current thread
 *
 * @see struct k_thread * z_current
 *
 * @return thread_t*
 */
static inline struct k_thread *k_thread_current(void)
{
	return z_current;
}

#ifdef __cplusplus
}
#endif

#endif