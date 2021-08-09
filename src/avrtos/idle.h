#ifndef _IDLE_H_
#define _IDLE_H_

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Allowing thread switch (k_yield) in interrupts require more stack as the interrupt already
 *  call-used registers (see https://gcc.gnu.org/wiki/avr-gcc).
 * 
 * TODO this need to be refactored
 * KERNEL_THREAD_IDLE_ADD_STACK : user additionnal stack, if doing a lot of things in interrupts handlers
 */
#if KERNEL_ALLOW_INTERRUPT_YIELD
#define K_TRHEAD_IDLE_STACK_SIZE K_THREAD_STACK_MIN_SIZE +                                    \
                                     (14u /* call-used registers */ + 3u /* max PC size */) + \
                                     10u + KERNEL_THREAD_IDLE_ADD_STACK
#else
#define K_TRHEAD_IDLE_STACK_SIZE K_THREAD_STACK_MIN_SIZE + 10u + KERNEL_THREAD_IDLE_ADD_STACK
#endif

    /*___________________________________________________________________________*/

    /**
 * @brief idle thread structure location
 */
    extern struct thread_t _k_idle;

    /**
 * @brief Idle thread entry function
 * 
 * @param context : ignored for now
 */
    void _k_idle_entry(void *context);

    /*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif