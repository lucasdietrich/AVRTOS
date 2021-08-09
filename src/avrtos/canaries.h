#ifndef _AVRTOS_CANARIES_H_
#define _AVRTOS_CANARIES_H_

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

/**
 * @brief Automatically initialize thread canaries in stack if configuration option THREAD_CANARIES is enabled.
 * Stack is filled with symbol THREAD_CANARIES_SYMBOL (default 0xAA = 0b10101010)
 */
void _k_init_thread_canaries(void);

/**
 * @brief Get the address of the furthest stack location used by the thread.
 * i.e. We loop until we found canaries for the thread {th}.
 * 
 * @param th 
 * @return void* Address of byte following the last found canary. (Address of the furthest stack location used by the thread.)
 */
void* _k_thread_canaries(struct thread_t *th);

/*___________________________________________________________________________*/

/**
 * @brief Pretty print found canaries in thread {th}
 * 
 * @param th thread to check
 */
void print_thread_canaries(struct thread_t *th);

/**
 * @brief Pretty print found canaries in the current thread
 * 
 * [M] CANARIES until @07CE [found 468], MAX usage = 44 / 512
 */
void print_current_canaries(void);

/**
 * @brief Pretty print found canaries in all threads
 * 
 * [M] CANARIES until @07CE [found 468], MAX usage = 44 / 512
 * [2] CANARIES until @010B [found 6], MAX usage = 506 / 512
 * [1] CANARIES until @03C7 [found 194], MAX usage = 62 / 256
 * [L] CANARIES until @04DE [found 217], MAX usage = 39 / 256
 * [K] CANARIES until @051F [found 26], MAX usage = 36 / 62
 * 
 */
void dump_threads_canaries(void);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif