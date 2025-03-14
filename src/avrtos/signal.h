/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Signals
 *
 * A signal is a synchronization mechanism that allows one thread to notify another
 * thread of an event. Unlike mutexes and semaphores, which are used to control access
 * to resources, signals are used to send simple notifications or events between
 * threads.
 *
 * Signals can be raised by one thread to indicate that an event has occurred, waking
 * up any threads that are waiting for that signal. Once a signal is raised, it
 * remains in the signaled state until it is manually reset.
 *
 * Particular care must be taken when using signals, as they are not automatically
 * reset after being handled. It is the responsibility of the user to reset the signal.
 *
 * In general, semaphores are better suited for signaling as we can *control* the number
 * of threads receiving the signal.
 *
 * Related configuration options:
 *  - CONFIG_KERNEL_ARGS_CHECKS: Enable argument checks
 */

#ifndef _AVRTOS_SIGNAL_H_
#define _AVRTOS_SIGNAL_H_

#include <stdbool.h>

#include <avr/io.h>

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Signal state definitions
 *
 * These constants define the possible states of a signal.
 */
#define K_POLL_STATE_NOT_READY 0x00
#define K_POLL_STATE_SIGNALED  0x01

/**
 * @brief Kernel Signal structure
 */
struct k_signal {
    /**
     * @brief The signal value.
     *
     * This value can be set when the signal is raised and is used to pass
     * information to the waiting thread(s).
     */
    uint8_t signal;

    /**
     * @brief Flags indicating the current state of the signal.
     *
     * This field indicates whether the signal is in the "signaled" state or not.
     */
    uint8_t flags;

    /**
     * @brief Wait queue for threads polling on the signal.
     *
     * Threads waiting for the signal are stored in this list, with the first
     * thread to be woken up at the head of the queue.
     */
    struct dnode waitqueue;
};

/**
 * @brief Statically initialize a signal.
 *
 * This macro initializes a signal at compile time, setting its initial
 * signal value to 0, its state to not ready, and initializing the wait queue.
 *
 * @param sig The signal structure to be initialized.
 */
#define Z_SIGNAL_INIT(sig)                                                               \
    {                                                                                    \
        .signal = 0u, .flags = K_POLL_STATE_NOT_READY,                                   \
        .waitqueue = DLIST_INIT(sig.waitqueue)                                           \
    }

/**
 * @brief Statically define and initialize a signal.
 *
 * This macro defines a signal structure and initializes it at compile time.
 *
 * @param signal_name Name of the signal structure.
 */
#define K_SIGNAL_DEFINE(signal_name)                                                     \
    struct k_signal signal_name = Z_SIGNAL_INIT(signal_name)

/**
 * @brief Reset a signal to the "not ready" state.
 *
 * This macro resets the state of the signal to indicate that it is not ready.
 *
 * @param signal Pointer to the signal structure.
 */
#define K_SIGNAL_SET_UNREADY(signal) (signal)->flags = K_POLL_STATE_NOT_READY

/**
 * @brief Initialize a signal object at runtime.
 *
 * This function initializes a signal structure, setting its initial signal
 * value to 0, its state to not ready, and initializing the wait queue.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param sig Pointer to the signal structure to be initialized.
 * @return 0 on success, or -EINVAL if the signal pointer is NULL.
 */
__kernel int8_t k_signal_init(struct k_signal *sig);

/**
 * @brief Raise a signal, waking up waiting threads.
 *
 * This function sets the signal value and wakes up any threads that are
 * waiting on the signal. The signal remains in the signaled state until manually reset.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param sig Pointer to the signal structure.
 * @param value The value to set for the signal.
 * @return The number of threads that were woken up.
 * 		   - -EINVAL if the signal pointer is NULL.
 * 		   - 0 if no threads were woken up.
 * 		   - >0 if one or more threads were woken up.
 */
__kernel int8_t k_signal_raise(struct k_signal *sig, uint8_t value);

/**
 * @brief Poll on a signal, with optional timeout.
 *
 * This function allows a thread to wait for a signal to be raised. If the signal
 * is already in the signaled state, the function returns immediately. Otherwise,
 * the thread waits until the signal is raised or the specified timeout expires.
 *
 * Note: The signal must be manually reset using K_SIGNAL_SET_UNREADY after it is handled.
 *
 * Safety: This function is generally not safe to call from an ISR context
 *         if the timeout is different from K_NO_WAIT.
 *
 * @param sig Pointer to the signal structure.
 * @param timeout Maximum time to wait for the signal. Use K_NO_WAIT for non-blocking
 * operation, or K_FOREVER to wait indefinitely.
 * @return 0 on success, or an error code otherwise:
 *         - -EINVAL if the signal pointer is NULL.
 *         - -ETIMEDOUT if the timeout expired before the signal was raised.
 */
__kernel int8_t k_poll_signal(struct k_signal *sig, k_timeout_t timeout);

/**
 * @brief Cancel the wait for all threads polling on the signal.
 *
 * This function cancels the wait for all threads currently polling on the signal,
 * removing them from the wait queue.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param sig Pointer to the signal structure.
 * @return The number of threads that were woken up.
 */
__kernel int8_t k_poll_cancel_wait(struct k_signal *sig);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_SIGNAL_H_ */
