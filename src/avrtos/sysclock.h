#ifndef _AVRTOS_SYSCLOCK
#define _AVRTOS_SYSCLOCK


/*___________________________________________________________________________*/

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

extern volatile uint8_t _sysclock_counter;

void init_sysclock(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif