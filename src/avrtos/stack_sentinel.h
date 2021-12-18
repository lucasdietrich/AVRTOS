#ifndef _AVRTOS_STACK_SENTINEL_H_
#define _AVRTOS_STACK_SENTINEL_H_

#include "avrtos.h"

#include <stdbool.h>
    
#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

void _k_init_stack_sentinel(void);

void _k_init_thread_stack_sentinel(struct k_thread *th);

bool k_verify_stack_sentinel(struct k_thread *th);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_STACK_SENTINEL_H_ */