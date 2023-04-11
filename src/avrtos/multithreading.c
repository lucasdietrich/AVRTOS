/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*___________________________________________________________________________*/

#include "kernel.h"
#include "multithreading.h"

/*___________________________________________________________________________*/

extern int main(void);

#if CONFIG_THREAD_EXPLICIT_MAIN_STACK == 1

__noinit char z_main_stack[CONFIG_THREAD_MAIN_STACK_SIZE];

Z_STACK_SENTINEL_REGISTER(z_main_stack);

#endif

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

struct k_thread *z_current = &z_thread_main;

/*___________________________________________________________________________*/

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
	if (stack_size < Z_THREAD_STACK_MIN_SIZE) {
		return -1;
	}

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

/*___________________________________________________________________________*/

inline struct k_thread *k_thread_current(void)
{
	return z_current;
}

/*___________________________________________________________________________*/