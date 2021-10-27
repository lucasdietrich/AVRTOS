#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/sysclock_config.h>

#if KERNEL_DEBUG_PREEMPT_UART == 0
.global _k_init_sysclock

_k_init_sysclock:
; order doesn't matter here
    ldi r24, SYSCLOCK_TIMER_TCNTL
    sts SYSCLOCK_HW_REG_TCNTXL, r24

#if KERNEL_SYSLOCK_HW_TIMER == 1
    ldi r24, SYSCLOCK_TIMER_TCNTH
    sts SYSCLOCK_HW_REG_TCNTXH, r24
#endif

    ldi r24, SYSCLOCK_HW_VAL_TCCRXA
    sts SYSCLOCK_HW_REG_TCCRXA, r24

    ldi r24, SYSCLOCK_HW_VAL_TCCRXB
    sts SYSCLOCK_HW_REG_TCCRXB, r24
    
#if KERNEL_SYSLOCK_HW_TIMER == 1
    ldi r24, SYSCLOCK_HW_VAL_TCCRXC
    sts SYSCLOCK_HW_REG_TCCRXC, r24
#endif

    ldi r24, SYSCLOCK_HW_VAL_TIMSKX
    sts SYSCLOCK_HW_REG_TIMSKX, r24
    
    ret

#endif