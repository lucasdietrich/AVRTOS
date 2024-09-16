/*
 * Stack sentinel
 *
 * Stack sentinels are specific byte patterns placed at the top of the stack to
 * detect overflows. By verifying these sentinels, the system can detect if a
 * stack overflow has occurred.
 *
 * If the sentinel is hit and the system monitor them with CONFIG_THREAD_MAIN_MONITOR=1
 * a fault is triggered with the reason K_FAULT_STACK_SENTINEL.
 *
 * Related configuration options:
 * - CONFIG_AVRTOS_LINKER_SCRIPT: Enable to use the linker script to statically
 *  initialize stack sentinels for all threads.
 * - CONFIG_THREAD_STACK_SENTINEL: Enable to use stack sentinels
 * - CONFIG_THREAD_STACK_SENTINEL_SIZE: The size of the stack sentinel in bytes.
 * - CONFIG_THREAD_STACK_SENTINEL_SYMBOL: The symbol used to fill the stack sentinel.
 */

#ifndef _AVRTOS_STACK_SENTINEL_H_
#define _AVRTOS_STACK_SENTINEL_H_

#include <stdbool.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Verify the integrity of the stack sentinel for a specific thread.
 *
 * This function checks if the stack sentinel byte(s) for the given thread
 * are still intact.
 *
 * @param thread Pointer to the thread whose stack sentinel is to be verified.
 * @return true if the stack sentinel is intact, false if it has been corrupted.
 */
bool k_verify_stack_sentinel(struct k_thread *thread);

/**
 * @brief Verify the integrity of stack sentinels for all registered threads.
 *
 * This function checks the stack sentinel byte(s) for all threads defined in the
 * .k_threads section.
 */
void k_assert_registered_stack_sentinel(void);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_STACK_SENTINEL_H_ */
