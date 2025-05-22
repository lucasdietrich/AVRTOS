/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stack_sentinel.h"

#include <avr/pgmspace.h>

#include "avrtos/sys.h"
#include "fault.h"

#if CONFIG_AVRTOS_LINKER_SCRIPT
extern struct k_thread __k_threads_start;
extern struct k_thread __k_threads_end;

void z_init_thread_stack_sentinel(struct k_thread *thread)
{
    uint8_t *const stack = K_STACK_START(thread->stack.end, thread->stack.size);
    for (uint8_t *addr = stack; addr < stack + CONFIG_THREAD_STACK_SENTINEL_SIZE;
         addr++) {
        *addr = CONFIG_THREAD_STACK_SENTINEL_SYMBOL;
    }
}

void z_init_stacks_sentinel(void)
{
    for (struct k_thread *thread = &__k_threads_start; thread < &__k_threads_end;
         thread++) {
        if (Z_THREAD_IS_MONITORED(thread)) {
            z_init_thread_stack_sentinel(thread);
        }
    }
}

extern void *__k_sent_start;
extern void *__k_sent_end;

void k_assert_registered_stack_sentinel(void)
{
    for (void **loc = &__k_sent_start; loc < &__k_sent_end; loc++) {
        void *sent = pgm_read_ptr(loc);
        for (void *p = sent; p < sys_ptr_add(sent, CONFIG_THREAD_STACK_SENTINEL_SIZE);
             p       = sys_ptr_inc(p)) {
            if (*(uint8_t *)p != CONFIG_THREAD_STACK_SENTINEL_SYMBOL) {
                __fault(K_FAULT_STACK_SENTINEL);
            }
        }
    }
}
#endif // CONFIG_AVRTOS_LINKER_SCRIPT

bool z_thread_verify_sent(struct k_thread *thread)
{
    uint8_t *const stack_sent = K_STACK_START(thread->stack.end, thread->stack.size);
    for (uint8_t *addr = stack_sent;
         addr < stack_sent + CONFIG_THREAD_STACK_SENTINEL_SIZE; addr++) {
        if (*addr != CONFIG_THREAD_STACK_SENTINEL_SYMBOL) {
            return false;
        }
    }

    return true;
}

bool k_verify_stack_sentinel(struct k_thread *thread)
{
    if (!Z_THREAD_IS_MONITORED(thread)) {
        return true;
    }

    return z_thread_verify_sent(thread);
}