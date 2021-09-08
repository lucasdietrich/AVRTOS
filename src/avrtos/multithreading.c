/*___________________________________________________________________________*/

#include "multithreading.h"

#include "kernel.h"

/*___________________________________________________________________________*/

extern int main(void);

#if THREAD_EXPLICIT_MAIN_STACK == 1

char _k_main_stack[THREAD_MAIN_STACK_SIZE];

#endif

K_THREAD struct k_thread _k_thread_main = {
    .sp = NULL, // main thread is running, context already "restored"
    {
        .flags = RUNNING | K_PRIO_PREEMPT(K_PRIO_MAX),
    },
    .tie = {
        .runqueue = {   // thread in before initialisation at the top of the runqueue (is actually the reference element)
            .prev = &_k_thread_main.tie.runqueue,
            .next = &_k_thread_main.tie.runqueue
        }
    },
    .wmutex = DITEM_INIT_NULL(),   // the thread isn't waiting of an y events
#if THREAD_EXPLICIT_MAIN_STACK == 1 // explicit stack defined, we set the main thread stack at the end of the defined buffer
    .stack = {
        .end = (void *)_K_STACK_END(_k_main_stack, THREAD_MAIN_STACK_SIZE),
        .size = THREAD_MAIN_STACK_SIZE,
    },
#else
    .stack = {          // implicit stack, we set the main thread stack end at the end of the RAM
        .end = (void*) RAMEND,
        .size = 0,
    },
#endif
    .local_storage = NULL,  // can be set later
    .symbol = 'M'           // default main thread sumbol
};

struct k_thread * k_current = &_k_thread_main;


/*___________________________________________________________________________*/

#if THREAD_USE_INIT_STACK_ASM == 0

void _k_thread_stack_create(struct k_thread *const th, thread_entry_t entry, void *const stack_end, void *const context_p)
{
    // get stack pointer value
    uint8_t* sp = (uint8_t*) stack_end - 1;

    // add return addr to stack (with format >> 1)
    *(uint16_t*)sp = K_SWAP_ENDIANNESS((uint16_t) entry);
    sp -= 1u;

    // push r0 > r23 (24 registers)
    for(uint_fast8_t i = 0u; i < 8u + _K_ARCH_STACK_SIZE_FIXUP; i++)
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

#include "misc/uart.h"

int k_thread_create(struct k_thread *const th, thread_entry_t entry, void *const stack, const size_t stack_size, const int8_t priority, void *const context_p, void *const local_storage, const char symbol)
{
    if (stack_size < K_THREAD_STACK_MIN_SIZE)
    {
        return -1;
    }
    
    th->stack.end = (void*) _K_STACK_END(stack, stack_size);

    _k_thread_stack_create(th, entry, th->stack.end, context_p);

    th->stack.size = stack_size;
    th->local_storage = local_storage;
    th->state = READY;
    th->priority = priority;
    th->symbol = symbol;

    _k_queue(th);

    return 0;
}

/*___________________________________________________________________________*/

inline struct k_thread * k_thread_current(void)
{
    return k_current;
}

inline void * k_thread_local_storage(void)
{
    return k_current->local_storage;
}
