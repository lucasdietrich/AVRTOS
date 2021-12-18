#ifndef _IDLE_H_
#define _IDLE_H_

#include <stddef.h>
#include <stdbool.h>

#include "avrtos.h"

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
 * @brief Tells if the runqueue contains the thread IDLE
 * 
 * @return true 
 * @return false 
 */

K_NOINLINE bool k_is_cpu_idle(void);

/**
 * @brief IDLE the CPU.
 * 
 * This function can be called from a thread to make it behave as an IDLE thread.
 * 
 * Important: if others thread a ready, they will be blocked until an interrupt occurs !
 * 
 * Note: This function forces interrupts to be enabled.
 */
K_NOINLINE void k_idle(void);


/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif