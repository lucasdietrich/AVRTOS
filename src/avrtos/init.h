#ifndef _AVRTOS_INIT_H_
#define _AVRTOS_INIT_H_

#include <avrtos/defines.h>
#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#if THREAD_EXPLICIT_MAIN_STACK == 1
__attribute__((naked, used, section(".init3"))) void _k_kernel_sp(void);
#endif


void __attribute__((naked, used, section(".init8"))) _k_avrtos_init(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif