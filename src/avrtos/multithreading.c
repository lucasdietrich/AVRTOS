/*___________________________________________________________________________*/

#include "multithreading.h"

#include "kernel.h"

/*___________________________________________________________________________*/

extern int main(void);

#if THREAD_EXPLICIT_MAIN_STACK == 1

char _k_main_stack[THREAD_MAIN_STACK_SIZE];

K_THREAD struct thread_t k_thread_main = {
    .sp = NULL,
    .tie = {.runqueue = {.prev = &k_thread_main.tie.runqueue, .next  = &k_thread_main.tie.runqueue}},
    {
        .flags = RUNNING | K_PRIO_PREEMPT(K_PRIO_MAX),
    },
    .stack = {
        .end = (void*) K_STACK_END(_k_main_stack, THREAD_MAIN_STACK_SIZE),
        .size = THREAD_MAIN_STACK_SIZE,
    },
    .local_storage = NULL,
    .symbol = 'M'
};

#else

K_THREAD struct thread_t k_thread_main = {
    .sp = NULL,
    .tie = {.runqueue = {.prev = &k_thread_main.tie.runqueue, .next = &k_thread_main.tie.runqueue}},
    {
        .flags = RUNNING | K_PRIO_PREEMPT(K_PRIO_MAX),
    },
    .stack = {
        .end = (void*) RAMEND,
        .size = 0,
    },
    .local_storage = NULL,
    .symbol = 'M'
};

#endif

extern struct thread_t __k_threads_start;

// remove this structure, only keep current variable
struct k_thread_meta k_thread = {
    .current = &k_thread_main,
    .list = &__k_threads_start,
    .count = 1
};

/*___________________________________________________________________________*/

#if THREAD_USE_INIT_STACK_ASM == 0

void _k_thread_stack_create(struct thread_t *const th, thread_entry_t entry, void *const stack_end, void *const context_p)
{
    // get stack pointer value
    uint8_t* sp = (uint8_t*) stack_end - 1;

    // add return addr to stack (with format >> 1)
    *(uint16_t*)sp = K_SWAP_ENDIANNESS((uint16_t) entry);
    sp -= 1u;

    // push r0 > r23 (24 registers)
    for(uint_fast8_t i = 0u; i < 8u; i++)
    {
        *sp-- = 0u;
    }

    // set context (p)
    sp -= 1u;
    *(uint16_t*)sp = K_SWAP_ENDIANNESS((uint16_t) context_p);
    sp -= 1u;

    // push r26 > r31 (6 registers)
    for (uint_fast8_t i = 0u; i < 22u; i++)
    {
        *sp-- = 0u;
    }

    // push sreg
    *sp = (uint8_t) THREAD_DEFAULT_SREG;
    sp -= 1u;

    // save SP in thread structure
    th->sp = sp;
}
#endif

int k_thread_create(struct thread_t *const th, thread_entry_t entry, void *const stack, const size_t stack_size, const int8_t priority, void *const context_p, void *const local_storage)
{
    if (stack_size < K_THREAD_STACK_MIN_SIZE)
    {
        return -1; // TODO return error
    }

    th->stack.end = (void*) K_STACK_END(stack, stack_size);

    _k_thread_stack_create(th, entry, th->stack.end, context_p);

    th->stack.size = stack_size;
    th->local_storage = local_storage;
    th->state = READY;
    th->priority = priority;

    k_queue(th);

    return 0;
}

/*___________________________________________________________________________*/

inline struct thread_t * k_thread_current(void)
{
    return k_thread.current;
}

//
// Utils
//
inline void * k_thread_local_storage(void)
{
    return k_thread_current()->local_storage;
}
