#ifndef _AVRTOS_SYSCLOCK
#define _AVRTOS_SYSCLOCK

/*___________________________________________________________________________*/

#include "multithreading.h"

/*___________________________________________________________________________*/

// TODO :  update
// as thread context switch is 26µs and it is not advice to do context switch more than 10% of the time
// maximum time slice must be 26µs*9 = 234µs ~ 4273 Hz

/*___________________________________________________________________________*/

#define KERNEL_TIME_SLICE_HZ            1000 / KERNEL_TIME_SLICE

#define SYSCLOCK_PRESCALER_FREQ_MIN(prescaler) ((F_CPU >> 8) / prescaler)
#define SYSCLOCK_PRESCALER_FREQ_MAX(prescaler) (F_CPU / prescaler)
#define SYSCLOCK_TCNT(time_ms, prescaler) (time_ms * (F_CPU / 1000) / prescaler)

/*___________________________________________________________________________*/

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Initialize and enable timer0 overflow vector used for preemptive threads 
 */
void _k_init_sysclock(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif