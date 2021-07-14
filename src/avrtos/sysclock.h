#ifndef _AVRTOS_SYSCLOCK
#define _AVRTOS_SYSCLOCK

/*___________________________________________________________________________*/

#include "multithreading.h"

/*___________________________________________________________________________*/

// as thread context switch is 26µs and it is not advice to do context switch more than 10% of the time
// maximum time slice must be 26µs*9 = 234µs ~ 4273 Hz

/*___________________________________________________________________________*/

#define KERNEL_TIME_SLICE_HZ            1000 / KERNEL_TIME_SLICE

#define SYSCLOCK_PRESCALER_1        ((1 << CS00) | (0 << CS01) |  (0 << CS02))
#define SYSCLOCK_PRESCALER_8        ((0 << CS00) | (1 << CS01) |  (0 << CS02))
#define SYSCLOCK_PRESCALER_64       ((1 << CS00) | (1 << CS01) |  (0 << CS02))
#define SYSCLOCK_PRESCALER_256      ((0 << CS00) | (0 << CS01) |  (1 << CS02))
#define SYSCLOCK_PRESCALER_1024     ((1 << CS00) | (0 << CS01) |  (1 << CS02))

#define SYSCLOCK_PRESCALER_FREQ_MIN(prescaler) ((F_CPU >> 8) / prescaler)
#define SYSCLOCK_PRESCALER_FREQ_MAX(prescaler) (F_CPU / prescaler)

#define SYSCLOCK_TIMER0_PRESCALER   SYSCLOCK_PRESCALER_1024

// TEST
// #define SYSCLOCK_TIMER0_TCNT0       0x100 - KERNEL_TIME_SLICE * ((int) ((F_CPU / 1000.0) / 1024.0 + 0.5))

/*___________________________________________________________________________*/

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

void _k_init_sysclock(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif