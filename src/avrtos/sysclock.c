#include <avrtos/misc/uart.h>
#include "sysclock.h"

#include "kernel.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "sysclock_config.h"

void k_init_sysclock(void)
{
	K_SYSCLOCK_HW_REG_TCNTXL = K_SYSCLOCK_TIMER_TCNTL;

#if KERNEL_SYSLOCK_HW_TIMER == 1
	K_SYSCLOCK_HW_REG_TCNTXH = K_SYSCLOCK_TIMER_TCNTH;
#endif /* KERNEL_SYSLOCK_HW_TIMER == 1 */

	K_SYSCLOCK_HW_REG_TCCRXA = K_SYSCLOCK_HW_VAL_TCCRXA;
	K_SYSCLOCK_HW_REG_TCCRXB = K_SYSCLOCK_HW_VAL_TCCRXB;

#if KERNEL_SYSLOCK_HW_TIMER == 1
	K_SYSCLOCK_HW_REG_TCCRXC = K_SYSCLOCK_HW_VAL_TCCRXC;
#endif /* KERNEL_SYSLOCK_HW_TIMER == 1 */
}

void k_start_sysclock(void)
{
	/* unmask sysclock interrupt */
	K_SYSCLOCK_HW_REG_TIMSKX = K_SYSCLOCK_HW_VAL_TIMSKX;
}