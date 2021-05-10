#ifndef _MULTITHREADING_DEBUG_H
#define _MULTITHREADING_DEBUG_H

#include "uart.h"
#include "utils.h"
#include "multithreading.h"

/*___________________________________________________________________________*/

// get current thread usage
uint16_t k_thread_usage(thread_t *th);

void k_thread_dbg_count(void);

void k_thread_dump_hex(thread_t *th);

void k_thread_dump(thread_t *th);

void k_thread_dump_all(void);


/*___________________________________________________________________________*/

#endif 