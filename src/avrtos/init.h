#ifndef _AVRTOS_INIT_H_
#define _AVRTOS_INIT_H_

#include <avrtos/defines.h>
#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#if KERNEL_AUTO_INIT == 1 && THREAD_EXPLICIT_MAIN_STACK == 1
__attribute__((naked, used, section(".init3")))
#endif
void _k_kernel_sp(void);

#if KERNEL_AUTO_INIT == 1
void __attribute__((naked, used, section(".init8")))
#endif
_k_avrtos_init(void);

/**
 * @brief Manual user AVRTOS init
 */
void k_avrtos_init(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif