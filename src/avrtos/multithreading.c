/*___________________________________________________________________________*/

#include "multithreading.h"

/*___________________________________________________________________________*/

extern int main(void);

#if THREAD_EXPLICIT_MAIN_STACK == 1

char _k_main_stack[THREAD_MAIN_STACK_SIZE];

struct thread_t k_thread_main = {
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

struct thread_t k_thread_main = {
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

struct k_thread_meta k_thread = {
    &k_thread_main,     // current thread is main
    .list = {           // TODO remove this list, and stack all thread in the same ld section (compilation/runtime declaration)
        &k_thread_main  // only known thread is main
    },
    .count = 1u,        // one known thread
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
    if (k_thread.count >= K_THREAD_MAX_COUNT)
    {
        return -1;
    }

    if (stack_size < K_THREAD_STACK_MIN_SIZE)
    {
        return -1; // TODO return error
    }

    k_thread.list[k_thread.count++] = th;
    th->stack.end = (void*) K_STACK_END(stack, stack_size);

    _k_thread_stack_create(th, entry, th->stack.end, context_p);

    th->stack.size = stack_size;
    th->local_storage = local_storage;
    th->state = READY;
    th->priority = priority;

    return 0;
}

int k_thread_register(struct thread_t *const th)
{
    uint8_t i;

    for(i = 0; i < k_thread.count; i++)
    {
        if (th == k_thread.list[i])
        {
            return 0; // ok (already registerd)
        }
    }

    // thread_t not found
    if (i < K_THREAD_MAX_COUNT)
    {
        k_thread.list[k_thread.count++] = th;  // we add it

        // TODO add thread to running queue

        return 0;
    }
    else
    {
        return -1;  // nok, no more space to register the thread
    }

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

/*___________________________________________________________________________*/

extern struct thread_t __k_threads_start;
extern uint8_t __data_end;

// naked remove "ret" instruction, compile as pure instructions

/**
 * @brief This function is called when system starts up, only first (K_THREAD_MAX_COUNT - 1) threads will be registeres 
 */
 __attribute__((used, naked, section(".init5"))) void _k_threads_register(void) 
{
    uint8_t i = 0;
    while ((uint16_t) &(&__k_threads_start)[i] < (uint16_t) &__data_end)
    {
        k_thread.list[k_thread.count++] = &(&__k_threads_start)[i++];
    }
}
