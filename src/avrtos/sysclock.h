#ifndef _AVRTOS_SYSCLOCK
#define _AVRTOS_SYSCLOCK

/*___________________________________________________________________________*/

#include <avr/io.h>

#include "defines.h"
#include "sysclock_config.h"

/*___________________________________________________________________________*/

// resolution is 8 or 16 bits
#define SYSCLOCK_TIMER_FREQ_MIN(prescaler, resolution) ((F_CPU >> resolution) / prescaler)
#define SYSCLOCK_TIMER_FREQ_MAX(prescaler) (F_CPU / prescaler)
#define SYSCLOCK_TIMER_TCNT_CALC(time_ms, prescaler) (time_ms * (F_CPU / 1000) / prescaler)

/*___________________________________________________________________________*/

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Initialize and enable timer overflow vector used for preemptive threads 
 */
void _k_init_sysclock(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif