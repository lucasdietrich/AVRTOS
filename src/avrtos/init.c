#include "init.h"

#include <avrtos/kernel.h>

#include "io.h"

/*___________________________________________________________________________*/

extern char _k_main_stack[];

/*___________________________________________________________________________*/

void _k_kernel_sp(void)
{
        SP = (uint16_t)_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE);
}

/*___________________________________________________________________________*/

void _k_avrtos_init(void)
{
#if KERNEL_DEBUG_PREEMPT_UART
        UCSR0B = 1 << RXCIE0;
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

#if KERNEL_SYSCLOCK_AUTO_INIT
        _k_init_sysclock();
#endif
}


void k_avrtos_init(void)
{
#if THREAD_EXPLICIT_MAIN_STACK == 1
        _k_kernel_sp();
#endif

        _k_avrtos_init();
}

/*___________________________________________________________________________*/
