/*___________________________________________________________________________*/

#include "multithreading.h"

#include "kernel.h"

/*___________________________________________________________________________*/

extern int main(void);

#if THREAD_EXPLICIT_MAIN_STACK == 1

char _k_main_stack[THREAD_MAIN_STACK_SIZE];

#endif

K_THREAD struct k_thread _k_thread_main = {
    .sp = 0, // main thread is running, context already "restored"
    {
#if THREAD_MAIN_COOPERATIVE == 1
        .flags = READY | K_COOPERATIVE,
#else
        .flags = READY | K_PREEMPTIVE,
#endif
    },
    .tie = {
        .runqueue = {   // thread in before initialisation at the top of the runqueue (is actually the reference element)
            .prev = &_k_thread_main.tie.runqueue,
            .next = &_k_thread_main.tie.runqueue
        }
    },
    .wany = DITEM_INIT_NULL(),   // the thread isn't pending on any events
    .swap_data = NULL,
#if THREAD_EXPLICIT_MAIN_STACK == 1 // explicit stack defined, we set the main thread stack at the end of the defined buffer
    .stack = {
        .end = (void *)_K_STACK_END(_k_main_stack, THREAD_MAIN_STACK_SIZE),
        .size = THREAD_MAIN_STACK_SIZE,
    },
#else
    .stack = {          // implicit stack, we set the main thread stack end at the end of the RAM
        .end = (void *)RAMEND,
        .size = 0,
    },
#endif
    .symbol = 'M'           // default main thread sumbol
};

struct k_thread * _current = &_k_thread_main;


/*___________________________________________________________________________*/

static void _k_thread_stack_create(struct k_thread *const th, thread_entry_t entry,
				   void *const context_p)
{
	struct _k_callsaved_ctx *const ctx = K_THREAD_CTX_START(th->stack.end);

	/* initialize unused registers with default value */
	for (uint8_t *reg = ctx->regs; reg < ctx->regs + sizeof(ctx->regs); reg++) {
		*reg = 0x00U;
	}
	
	ctx->sreg = THREAD_DEFAULT_SREG;
	ctx->thread_context = (void*) K_SWAP_ENDIANNESS(context_p);
	ctx->thread_entry = (void*) K_SWAP_ENDIANNESS(entry);
	ctx->pc = (void*) K_SWAP_ENDIANNESS(_k_thread_entry);

#if __AVR_3_BYTE_PC__
	ctx->pch = 0;
#endif

        /* save SP in thread structure */
        th->sp = ctx;

	/* adjust pointer to the top of the stack */
	th->sp--;
}

#include "misc/uart.h"

int k_thread_create(struct k_thread *const th, thread_entry_t entry,
        void *const stack, const size_t stack_size,
        const int8_t prio, void *const context_p, const char symbol)
{
        if (stack_size < K_THREAD_STACK_MIN_SIZE) {
                return -1;
        }

        th->stack.end = (void *)_K_STACK_END(stack, stack_size);
	th->stack.size = stack_size;

        _k_thread_stack_create(th, entry, context_p);

#if THREAD_STACK_SENTINEL
	_k_init_thread_stack_sentinel(th);
#endif /* THREAD_STACK_SENTINEL */

#if THREAD_CANARIES
	_k_init_thread_stack_canaries(th);
#endif /* THREAD_CANARIES */

        /* clear internal flags */
        th->flags = 0;
        th->state = STOPPED;
        th->coop = prio & K_FLAG_COOP ? 1 : 0;
        th->priority = prio & K_FLAG_PRIO;
        th->symbol = symbol;
        th->swap_data = NULL;

        return 0;
}

/*___________________________________________________________________________*/

inline struct k_thread *k_thread_current(void)
{
        return _current;
}

/*___________________________________________________________________________*/