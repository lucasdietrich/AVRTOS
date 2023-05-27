/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "kernel.h"
#include "multithreading.h"

extern int main(void);

#if CONFIG_THREAD_EXPLICIT_MAIN_STACK == 1

__noinit char z_main_stack[CONFIG_THREAD_MAIN_STACK_SIZE];

Z_STACK_SENTINEL_REGISTER(z_main_stack);

#endif

/**
 * @brief Main thread instance.
 *
 * This variable represents the main thread of the application.
 *
 * The field initializations are as follows:
 * - `.sp`: Set to 0 to indicate that the main thread is running and no context
 *   is currently saved.
 * - `.flags`: Combination of flags indicating the state and priority of the
 *   thread. The specific combination depends on the configuration options.
 * - `.tie`: Contains the reference element for the thread in the runqueue.
 * - `.wany`: Initialized to indicate that the thread is not pending on any events.
 * - `.swap_data`: Set to NULL as there is no explicit swapping data associated with
 *   the main thread.
 * - `.stack`: Contains information about the thread's stack. The specific values
 *   depend on the configuration options, such as whether an explicit stack is defined
 *   or an implicit stack is used.
 * - `.symbol`: Default symbol for the main thread, here set to 'M'.
 */
K_THREAD struct k_thread z_thread_main = {
	.sp = 0,  // main thread is running, context already "restored"
#if CONFIG_THREAD_MAIN_COOPERATIVE == 1
	.flags = Z_THREAD_STATE_READY | Z_THREAD_PRIO_COOP | Z_THREAD_PRIO_LOW,
#else
	.flags = Z_THREAD_STATE_READY | Z_THREAD_PRIO_PREEMPT | Z_THREAD_PRIO_LOW,
#endif
	.tie =
		{
			.runqueue =
				{
					// thread in before initialisation at
					// the top of the runqueue (is actually
					// the reference element)
					.prev = &z_thread_main.tie.runqueue,
					.next = &z_thread_main.tie.runqueue,
				},
		},
	.wany	   = DITEM_INIT_NULL(),	 // the thread isn't pending on any events
	.swap_data = NULL,
#if CONFIG_THREAD_EXPLICIT_MAIN_STACK == \
	1  // explicit stack defined, we set the main thread stack at the end of
	   // the defined buffer
	.stack =
		{
			.end  = (void *)Z_STACK_END(z_main_stack,
						    CONFIG_THREAD_MAIN_STACK_SIZE),
			.size = CONFIG_THREAD_MAIN_STACK_SIZE,
		},
#else
	.stack =
		{
			// implicit stack, we set the main thread stack end at
			// the end of the RAM
			.end  = (void *)RAMEND,
			.size = CONFIG_THREAD_MAIN_STACK_SIZE, /* Used as
								  indication
								  only */
		},
#endif
	.symbol = 'M'  // default main thread sumbol
};

/**
 * @brief Pointer to the thread currently having the CPU (context set).
 *
 * @note there are cases where z_runq and z_current doesn't refer to the same thread
 * this is because the next thread to be scheduled is already set but the context hasn't
 * been switched yet.
 */
struct k_thread *z_current = &z_thread_main;

static void z_thread_stack_create(struct k_thread *const thread,
				  thread_entry_t entry,
				  void *const context_p)
{
	struct z_callsaved_ctx *const ctx = Z_THREAD_CTX_START(thread->stack.end);

	/* initialize unused registers with default value */
	for (uint8_t *reg = ctx->regs; reg < ctx->regs + sizeof(ctx->regs); reg++) {
		*reg = 0x00u;
	}

	ctx->sreg	    = 0U;
	ctx->init_sreg	    = CONFIG_THREAD_DEFAULT_SREG;
	ctx->thread_context = (void *)K_SWAP_ENDIANNESS(context_p);
	ctx->thread_entry   = (void *)K_SWAP_ENDIANNESS(entry);
	ctx->pc		    = (void *)K_SWAP_ENDIANNESS(z_thread_entry);

#if defined(__AVR_3_BYTE_PC__)
	ctx->pch = 0;
#endif

	/* save SP in thread structure */
	thread->sp = ctx;

	/* adjust pointer to the top of the stack */
	thread->sp--;
}

int8_t k_thread_create(struct k_thread *thread,
		       thread_entry_t entry,
		       void *stack,
		       size_t stack_size,
		       uint8_t prio,
		       void *context_p,
		       char symbol)
{
#if CONFIG_KERNEL_ARGS_CHECKS
	if (!thread || !entry || !stack || stack_size < Z_THREAD_STACK_MIN_SIZE) {
		return -EINVAL;
	}
#endif

	thread->stack.end  = (void *)Z_STACK_END(stack, stack_size);
	thread->stack.size = stack_size;

#if CONFIG_THREAD_CANARIES
	z_init_thread_stack_canaries(thread);
#endif /* CONFIG_THREAD_CANARIES */

#if CONFIG_THREAD_STACK_SENTINEL
	z_init_thread_stack_sentinel(thread);
#endif /* CONFIG_THREAD_STACK_SENTINEL */

	z_thread_stack_create(thread, entry, context_p);

	/* initialize internal data */
	thread->flags	  = Z_THREAD_STATE_STOPPED | (prio & Z_THREAD_PRIO_MSK);
	thread->symbol	  = symbol;
	thread->swap_data = NULL;

	return 0;
}