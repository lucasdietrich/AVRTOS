#ifndef _AVRTOS_INIT_H_
#define _AVRTOS_INIT_H_

#include <avrtos/defines.h>
#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#define K_KERNEL_LINK_SP_INIT() \
	__attribute__((naked, used, section(".init3"))) void _k_kernel_sp(void) \
	{ \
		extern char _k_main_stack[]; \
		SP = (uint16_t)_K_STACK_END_ASM(_k_main_stack, THREAD_MAIN_STACK_SIZE); \
	}

#define K_KERNEL_LINK_AVRTOS_INIT() \
	__attribute__((naked, used, section(".init8"))) void k_avrtos_init(void) \
	{ \
		_k_avrtos_init(); \
	}

#define K_KERNEL_LINK_INIT() \
	K_KERNEL_LINK_SP_INIT() \
	K_KERNEL_LINK_AVRTOS_INIT()

#define K_KERNEL_INIT K_KERNEL_LINK_INIT

#define K_AVRTOS_INIT	K_KERNEL_INIT

/**
 * @brief Manual user AVRTOS init
 */
void _k_avrtos_init(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif