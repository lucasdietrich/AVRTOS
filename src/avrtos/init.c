#include "init.h"

#include <avrtos/kernel.h>

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

        _k_kernel_init();
        _k_mem_slab_init_module();

#if KERNEL_TIMERS
        _k_timer_init_module();
#endif

#if THREAD_CANARIES
        _k_init_thread_canaries();
#endif

#if KERNEL_SYSCLOCK_AUTO_INIT
        _k_init_sysclock();
#endif
}

/*___________________________________________________________________________*/
