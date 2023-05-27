/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_KERNEL_TYPES_H
#define _AVRTOS_KERNEL_TYPES_H

#include "defines.h"
#include "dstruct/dlist.h"
#include "dstruct/tqueue.h"

#include <stddef.h>

/**
 * @brief Thread entry point function type.
 *
 * The `thread_entry_t` type is a function pointer type representing a thread entry
 * point function. A thread entry point function is a function that serves as the
 * starting point for the execution of a thread. It takes a single `void*` parameter
 * used to pass the thread context when the entry function is called.
 *
 * @param context A pointer to the context data for the thread.
 */
typedef void (*thread_entry_t)(void *);

/**
 * @brief Structure representing a thread.
 *
 * The `struct k_thread` structure represents a thread and defines various properties
 * associated with it, including the stack pointer, thread priority, stack location and
 * size, local storage pointer, and other relevant fields.
 *
 * This structure is 16 bytes long.
 */
struct k_thread {
	void *sp;  // stack pointer, TO KEEP it at the beginning of the structure !

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
 * @brief Structure representing the call-saved registers + PC (size=19 bytes)
 *
 * Read "Call-used registers" of the AVR GCC manual for more information:
 * https://gcc.gnu.org/wiki/avr-gcc#Call-Saved_Registers
 */
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

/**
 * @brief Structure representing the call-used register of a function call.
 *
 * Read "Call-used registers" of the AVR GCC manual for more information:
 * https://gcc.gnu.org/wiki/avr-gcc#Call-Used_Registers
 */
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

/**
 * @brief Represent the registers to be saved during an interrupt before calling the ISR.
 */
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

#endif /* _AVRTOS_KERNEL_TYPES_H */