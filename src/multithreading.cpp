#include "multithreading.h"

/*___________________________________________________________________________*/

thread_t k_thread_main = {
    .sp = nullptr,
    .tid = 1,
    .priority = -1, // cooperative
    .stack = {
        .end = RAMEND,
        .size = 0,  // undefined stack size size
    },
};

struct k_thread_meta k_thread = {
    .current = &k_thread_main,
    .list = {
        &k_thread_main,
        nullptr
    },
    .count = 1u,
    .current_idx = 0u,
};

/*___________________________________________________________________________*/

void k_thread_create(thread_t * th, thread_entry_t entry, uint16_t stack, size_t stack_size, int8_t priority, void *p)
{
    k_thread_create(th, entry, stack, p);

    th->stack.end = stack;
    th->stack.size = stack_size;

    th->priority = priority;

    k_thread.list[k_thread.count++] = th;
}

thread_t *k_schedule(void)
{
    // eval next thread to be exectued
    k_thread.current_idx = (k_thread.current_idx + 1) % k_thread.count;

    // set it as current thread
    k_thread.current = k_thread.list[k_thread.current_idx];

    // go back to yield and restore thread context
    return k_thread.current;
}

/*___________________________________________________________________________*/

//
// Utils
//

/*___________________________________________________________________________*/