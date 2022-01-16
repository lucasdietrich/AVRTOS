#ifndef _AVRTOS_SYSCLOCK
#define _AVRTOS_SYSCLOCK

/*___________________________________________________________________________*/

#include <avr/io.h>

#include "defines.h"

/*___________________________________________________________________________*/

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Initialize and enable timer overflow vector used for preemptive threads 
 */
K_NOINLINE void _k_init_sysclock(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif