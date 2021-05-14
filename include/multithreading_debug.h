#ifndef _MULTITHREADING_DEBUG_H
#define _MULTITHREADING_DEBUG_H

#include "uart.h"
#include "utils.h"
#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

// get current thread usage
uint16_t k_thread_usage(struct thread_t *th);
void k_thread_dbg_count(void);
void k_thread_dump_hex(struct thread_t *th);
void k_thread_dump(struct thread_t *th);
void k_thread_dump_all(void);
void *k_thread_get_return_addr(struct thread_t *th);
int k_thread_copy_registers(struct thread_t *th, char *const buffer, const size_t size);

#ifdef __cplusplus
}
#endif

#endif 