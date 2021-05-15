#include "multithreading.h"

/*___________________________________________________________________________*/

extern int main(void);

#if THREAD_EXPLICIT_MAIN_STACK == 1
char _k_main_stack[THREAD_MAIN_STACK_SIZE];

struct thread_t k_thread_main = {
    .sp = NULL,
    .priority = THREAD_MAIN_THREAD_PRIORITY,
    .stack = {
        .end = (void*) K_STACK_END(_k_main_stack, THREAD_MAIN_STACK_SIZE),
        .size = THREAD_MAIN_STACK_SIZE,
    },
    .local_storage = NULL,
};

#else
struct thread_t k_thread_main = {
    .sp = NULL,
    .priority = THREAD_MAIN_THREAD_PRIORITY,
    .stack = {
        .end = (void*) RAMEND,
        .size = 0,
    },
    .local_storage = NULL,
};

#endif

struct k_thread_meta k_thread = {
    &k_thread_main,     // current thread is main
    .list = {
        &k_thread_main  // only known thread is main
    },
    .count = 1u,        // one known thread
    .current_idx = 0u,
};

/*___________________________________________________________________________*/

#if THREAD_USE_INIT_STACK_ASM == 0

void _k_thread_stack_create(struct thread_t *const th, thread_entry_t entry, void *const stack_end, void *const context_p)
{
    // get stack pointer value
    uint8_t* sp = (uint8_t*) stack_end - 1;

    // add return addr to stack (with format >> 1)
    *(uint16_t*)sp = K_SWAP_LITTLE_BIG_ENDIAN((uint16_t) entry);
    sp -= 1u;

    // push r0 > r23 (24 registers)
    for(uint_fast8_t i = 0u; i < 24u; i++)
    {
        *sp-- = 0u;
    }

    // set context (p)
    sp -= 1u;
    *(uint16_t*)sp = (uint16_t) context_p;
    sp -= 1u;

    // push r26 > r31 (6 registers)
    for (uint_fast8_t i = 0u; i < 6u; i++)
    {
        *sp-- = 0u;
    }

    // push sreg
    *sp = (uint8_t) K_THREAD_DEFAULT_SREG;
    sp -= 1u;

    // save SP in thread structure
    th->sp = sp;
}
#endif

void k_thread_create(struct thread_t *const th, thread_entry_t entry, void *const stack, const size_t stack_size, const int8_t priority, void *const context_p, void *const local_storage)
{
    if (k_thread.count >= K_THREAD_MAX_COUNT)
    {
        return; // TODO return error
    }

    if (stack_size < K_THREAD_STACK_MIN_SIZE)
    {
        return; // TODO return error
    }

    k_thread.list[k_thread.count++] = th;
    th->stack.end = (void*) K_STACK_END(stack, stack_size);

    _k_thread_stack_create(th, entry, th->stack.end, context_p);

    th->stack.size = stack_size;
    th->priority = priority;
    th->local_storage = local_storage;
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

        return 0;
    }
    else
    {
        return -1;  // nok, no more space to register the thread
    }

}

struct thread_t *_k_scheduler(void)
{
    // eval next thread to be executed
    k_thread.current_idx = (k_thread.current_idx + 1) % k_thread.count;

    // set current
    k_thread.current = k_thread.list[k_thread.current_idx];

    // go back to yield and restore thread context
    // another solution would be to return anything and let the k_yield asm function
    // to retrieve the current thread from k_thread.current
    return k_thread.current;
}

/*___________________________________________________________________________*/

//
// Utils
//

inline void * k_thread_local_storage(void)
{
    return k_thread.current->local_storage;
}

/*___________________________________________________________________________*/