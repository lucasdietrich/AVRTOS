/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "defines.h"
#include "drivers/timer.h"

#if !CONFIG_KERNEL_TICKLESS || 1

#if (CONFIG_KERNEL_SYSCLOCK_PERIOD_US < 100)
#warning SYSCLOCK is probably too fast !
#endif

#if (CONFIG_KERNEL_TIME_SLICE_US < 500)
#warning CONFIG_KERNEL_TIME_SLICE_US is probably too short !
#endif

#if CONFIG_KERNEL_SYSLOCK_HW_TIMER >= TIMERS_COUNT
#error "invalid CONFIG_KERNEL_SYSLOCK_HW_TIMER"
#endif

#define TIMER_MAX_COUNTER TIMER_GET_MAX_COUNTER(CONFIG_KERNEL_SYSLOCK_HW_TIMER)

#if TIMER_COUNTER_VALUE_FIT(CONFIG_KERNEL_SYSCLOCK_PERIOD_US, 1LU, TIMER_MAX_COUNTER)
#define PRESCALER_VALUE 1
#elif TIMER_COUNTER_VALUE_FIT(CONFIG_KERNEL_SYSCLOCK_PERIOD_US, 8LU, TIMER_MAX_COUNTER)
#define PRESCALER_VALUE 8
#elif TIMER_COUNTER_VALUE_FIT(CONFIG_KERNEL_SYSCLOCK_PERIOD_US, 32LU,                    \
                              TIMER_MAX_COUNTER) &&                                      \
    (CONFIG_KERNEL_SYSLOCK_HW_TIMER == 2)
#define PRESCALER_VALUE 32
#elif TIMER_COUNTER_VALUE_FIT(CONFIG_KERNEL_SYSCLOCK_PERIOD_US, 64LU, TIMER_MAX_COUNTER)
#define PRESCALER_VALUE 64
#elif TIMER_COUNTER_VALUE_FIT(CONFIG_KERNEL_SYSCLOCK_PERIOD_US, 128LU,                   \
                              TIMER_MAX_COUNTER) &&                                      \
    (CONFIG_KERNEL_SYSLOCK_HW_TIMER == 2)
#define PRESCALER_VALUE 128
#elif TIMER_COUNTER_VALUE_FIT(CONFIG_KERNEL_SYSCLOCK_PERIOD_US, 256LU, TIMER_MAX_COUNTER)
#define PRESCALER_VALUE 256
#elif TIMER_COUNTER_VALUE_FIT(CONFIG_KERNEL_SYSCLOCK_PERIOD_US, 1024LU, TIMER_MAX_COUNTER)
#define PRESCALER_VALUE 1024
#else
#error "CONFIG_KERNEL_SYSCLOCK_PERIOD_US is too big for the selected timer"
#endif

#if CONFIG_KERNEL_SYSLOCK_HW_TIMER == 2

#if PRESCALER_VALUE == 1
#define PRESCALER_CONFIG TIMER2_PRESCALER_1
#elif PRESCALER_VALUE == 8
#define PRESCALER_CONFIG TIMER2_PRESCALER_8
#elif PRESCALER_VALUE == 32
#define PRESCALER_CONFIG TIMER2_PRESCALER_32
#elif PRESCALER_VALUE == 64
#define PRESCALER_CONFIG TIMER2_PRESCALER_64
#elif PRESCALER_VALUE == 128
#define PRESCALER_CONFIG TIMER2_PRESCALER_128
#elif PRESCALER_VALUE == 256
#define PRESCALER_CONFIG TIMER2_PRESCALER_256
#elif PRESCALER_VALUE == 1024
#define PRESCALER_CONFIG TIMER2_PRESCALER_1024
#else
#error "invalid PRESCALER_VALUE (timer 2)"
#endif

#else

#if PRESCALER_VALUE == 1
#define PRESCALER_CONFIG TIMER_PRESCALER_1
#elif PRESCALER_VALUE == 8
#define PRESCALER_CONFIG TIMER_PRESCALER_8
#elif PRESCALER_VALUE == 64
#define PRESCALER_CONFIG TIMER_PRESCALER_64
#elif PRESCALER_VALUE == 256
#define PRESCALER_CONFIG TIMER_PRESCALER_256
#elif PRESCALER_VALUE == 1024
#define PRESCALER_CONFIG TIMER_PRESCALER_1024
#else
#error "invalid PRESCALER_VALUE (timers 1, 3, 4, 5)"
#endif

#endif /* CONFIG_KERNEL_SYSLOCK_HW_TIMER == 2 */

#if (F_CPU * CONFIG_KERNEL_SYSCLOCK_PERIOD_US) % (PRESCALER_VALUE * 1000000LU) != 0
#warning "Sysclock may not be accurate"
#endif

// in case of qemu emulator, check if the timer is available
#if defined(__QEMU__) && TIMER_INDEX_IS_8BITS(CONFIG_KERNEL_SYSLOCK_HW_TIMER)
#error "QEMU emulator detected, only 16 bits timers are available"
#elif CONFIG_KERNEL_SYSLOCK_HW_TIMER >= 4
#warning                                                                                 \
    "In order to use timer 4 or 5 with QEMU <= 8.0.2, you'll need to apply patch located at \
scripts/patches/0001-Fix-handling-of-AVR-interrupts-above-33-by-switching.patch to qemu"
#endif

__STATIC_ASSERT_NOMSG(Z_KERNEL_TIME_SLICE_TICKS != 0);

#define COUNTER_VALUE                                                                    \
    TIMER_CALC_COUNTER_VALUE(CONFIG_KERNEL_SYSCLOCK_PERIOD_US, PRESCALER_VALUE)

void z_init_sysclock(void)
{
    void *const dev = timer_get_device(CONFIG_KERNEL_SYSLOCK_HW_TIMER);

    const struct timer_config cfg = {
        .mode      = TIMER_MODE_CTC,
        .prescaler = PRESCALER_CONFIG,
        .counter   = COUNTER_VALUE,
        .timsk     = BIT(OCIEnA),
    };

#if TIMER_INDEX_IS_16BIT(CONFIG_KERNEL_SYSLOCK_HW_TIMER)
    ll_timer16_init(dev, CONFIG_KERNEL_SYSLOCK_HW_TIMER, &cfg);
#elif TIMER_INDEX_IS_8BIT(CONFIG_KERNEL_SYSLOCK_HW_TIMER)
    ll_timer8_init(dev, CONFIG_KERNEL_SYSLOCK_HW_TIMER, &cfg);
#else
#error "invalid timer type"
#endif
}

#endif /* !CONFIG_KERNEL_TICKLESS */