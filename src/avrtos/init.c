/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "canaries.h"
#include "debug.h"
#include "init.h"
#include "io.h"
#include "kernel.h"
#include "kernel_internals.h"
#include "mem_slab.h"
#include "stack_sentinel.h"
#include "timer.h"

extern void z_init_sysclock(void);

extern void z_kernel_init(void);

void z_avrtos_init(void)
{
	/* Page 52 (ATmega328p datasheet) :
	 *	Note: If the Watchdog is accidentally enabled, for example by a
	 *runaway pointer or brown-out condition, the device will be reset and
	 *the Watchdog Timer will stay enabled. If the code is not set up to
	 *handle the Watchdog, this might lead to an eternal loop of time-out
	 *resets. To avoid this situation, the application software should
	 * always clear the Watchdog System Reset Flag (WDRF) and the WDE
	 *control bit in the initialization routine, even if the Watchdog is not
	 *in use.
	 */

	/* If the watchdog caused the reset, clear the flag */
	// if (MCUSR & BIT(WDRF)) {
	// 	MCUSR &= ~BIT(WDRF);
	// 	WDTCSR |= (_BV(WDCE) | _BV(WDE)); // change
	// 	WDTCSR = 0x00;
	// }

	/* Debug pins */
	__Z_DBG_GPIO_0_INIT();
	__Z_DBG_GPIO_1_INIT();
	__Z_DBG_GPIO_2_INIT();
	__Z_DBG_GPIO_3_INIT();

#if CONFIG_KERNEL_DEBUG_PREEMPT_UART
	SET_BIT(UCSR0B, BIT(RXCIE0));
#endif

	/* Send output stream to usart0 */
	k_set_stdio_usart0();

	z_kernel_init();

#if CONFIG_AVRTOS_LINKER_SCRIPT
	z_mem_slab_init_module();
#endif

#if CONFIG_KERNEL_TIMERS && CONFIG_AVRTOS_LINKER_SCRIPT
	z_timer_init_module();
#endif

#if CONFIG_THREAD_CANARIES && CONFIG_AVRTOS_LINKER_SCRIPT
	z_init_stacks_canaries();
#endif

#if CONFIG_THREAD_STACK_SENTINEL
	z_init_stacks_sentinel();
#endif

	z_init_sysclock();

#if (CONFIG_INTERRUPT_POLICY == 2) && (CONFIG_THREAD_MAIN_COOPERATIVE == 0)
	k_sched_lock();
#endif

#if CONFIG_INTERRUPT_POLICY > 0
	sei();
#endif
}

/**
 * @brief Automatically initialize AVRTOS if Arduino framework is used
 */
#if CONFIG_ARDUINO_FRAMEWORK
void initVariant(void)
{
	z_avrtos_init();
}
#endif /* ARDUINO */
