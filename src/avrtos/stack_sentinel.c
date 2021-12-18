#include "stack_sentinel.h"

extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

void _k_init_thread_stack_sentinel(struct k_thread *th)
{
	uint8_t *const stack = K_STACK_START(th->stack.end, th->stack.size);
	for (uint8_t *addr = stack;
	     addr < stack + THREAD_STACK_SENTINEL_SIZE; addr++) {
		*addr = THREAD_STACK_SENTINEL_SYMBOL;
	}
}

void _k_init_stacks_sentinel(void)
{
	for (struct k_thread *thread = &__k_threads_start;
	     thread < &__k_threads_end; thread++) {
		_k_init_thread_stack_sentinel(thread);
	}
}

bool k_verify_stack_sentinel(struct k_thread *th)
{
	uint8_t *const stack = K_STACK_START(th->stack.end, th->stack.size);
	for (uint8_t *addr = stack;
	     addr < stack + THREAD_STACK_SENTINEL_SIZE; addr++) {
		if (*addr != THREAD_STACK_SENTINEL_SYMBOL) {
			return false;
		}
	}
	return true;
}