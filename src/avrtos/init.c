#include "init.h"

#include <avrtos/kernel.h>

#include "io.h"


/*___________________________________________________________________________*/

void _k_avrtos_init(void)
{
	/* Page 52 (ATmega328p datasheet) :
	 *	Note: If the Watchdog is accidentally enabled, for example by a runaway pointer or brown-out condition, the
	 * device will be reset and the Watchdog Timer will stay enabled. If the code is not set up to handle the Watchdog,
	 * this might lead to an eternal loop of time-out resets. To avoid this situation, the application software should
	 * always clear the Watchdog System Reset Flag (WDRF) and the WDE control bit in the initialization routine,
	 * even if the Watchdog is not in use.
	 */

	/* If the watchdog caused the reset, clear the flag */
	// if (MCUSR & BIT(WDRF)) {
	// 	MCUSR &= ~BIT(WDRF);
	// 	WDTCSR |= (_BV(WDCE) | _BV(WDE)); // change
	// 	WDTCSR = 0x00;
	// }

#if KERNEL_DEBUG_PREEMPT_UART
	SET_BIT(UCSR0B, BIT(RXCIE0));
#endif

	/* Send output stream to usart0 */
	k_set_stdio_usart0();

        _k_kernel_init();
        _k_mem_slab_init_module();

#if KERNEL_TIMERS
        _k_timer_init_module();
#endif

#if THREAD_CANARIES
        _k_init_stacks_canaries();
#endif

#if THREAD_STACK_SENTINEL
        _k_init_stacks_sentinel();
#endif

	k_init_sysclock();

#if KERNEL_SYSCLOCK_AUTO_START
	k_start_sysclock();
#endif	
}

/*___________________________________________________________________________*/
