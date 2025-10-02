/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Flags (Event Flags)
 *
 * Flags, or event flags, are synchronization primitives used to manage and coordinate
 * the execution of threads based on the setting or clearing of specific bits. Each flag
 * is represented by a bit in a flags object, and threads can wait for certain conditions
 * (specific bits being set or cleared) before proceeding with their execution.
 *
 * Flags are particularly useful in scenarios where multiple events or conditions need
 * to be tracked independently or in combination. For example, a thread may wait for
 * any one of several events to occur or require that all specified events happen before
 * proceeding.
 *
 * Key Features:
 * - **Bitmask Operations**: Flags can be set, cleared, and polled using bitmask
 * operations, allowing for fine-grained control over thread synchronization.
 * - **Thread Notifications**: When the specified conditions (flags) are met, threads
 *   waiting on those flags are woken up, enabling responsive and efficient task
 * coordination.
 * - **Options and Customization**: Flags operations can be customized with options
 *   such as consuming the flags after use or scheduling a reschedule when a thread is
 * woken up.
 *
 * Example Usage:
 *
 *  - A thread might wait for any bit in a specific mask to be set, using options like
 *    `K_FLAGS_SET_ANY`. Once any of these bits are set by another thread or ISR, the
 *    waiting thread is woken up.
 *  - Another thread might notify the flags object by setting specific bits, possibly
 *    triggering multiple waiting threads to resume execution.
 *
 * Related configuration options:
 *  - CONFIG_KERNEL_FLAGS_SIZE: Specifies the size (in bytes) of the flags object,
 *    supporting 1 flag byte only. Future versions may support larger flags objects.
 * - CONFIG_KERNEL_ARGS_CHECKS: Enable argument checks for flags operations.
 */

#ifndef _AVRTOS_FLAGS_H
#define _AVRTOS_FLAGS_H

#include "kernel.h"

#if CONFIG_KERNEL_FLAGS_SIZE == 1
typedef uint8_t k_flags_value_t;
#else
#error "Unsupported flags size, only 1 byte is supported. Feature coming soon!"
#endif

/**
 * @brief Kernel Flags structure
 *
 * A flags object manages and synchronizes thread operations based on the setting
 * or clearing of specific bits (flags). Threads can wait for specific flags to
 * be set or cleared and can be notified when a change occurs.
 */
struct k_flags {
    struct dnode waitqueue;      ///< Wait queue for threads waiting on the flags
    k_flags_value_t flags;       ///< Current flags state
    k_flags_value_t reset_value; ///< Reset value for the flags
};

/**
 * @brief Statically initialize a flags object.
 *
 * This macro initializes a flags object at compile time with a specified
 * initial value and an empty wait queue.
 *
 * @param flags_name Name of the flags object.
 * @param initial_value Initial value of the flags.
 */
#define Z_FLAGS_INIT(flags_name, initial_value)                                          \
    {                                                                                    \
        .flags = initial_value, .reset_value = initial_value,                            \
        .waitqueue = DLIST_INIT(flags_name.waitqueue)                                    \
    }

/**
 * @brief Statically define and initialize a flags object.
 *
 * This macro defines a flags structure and initializes it at compile time.
 *
 * @param flags_name Name of the flags structure.
 * @param initial_value Initial value of the flags.
 */
#define K_FLAGS_DEFINE(flags_name, initial_value)                                        \
    struct k_flags flags_name = Z_FLAGS_INIT(flags_name, initial_value)

/**
 * @brief Options for flag operations.
 *
 * These options control how flags are polled and notified.
 */
typedef enum {
    /* Polling options */
    K_FLAGS_SET_ANY = 1 << 0u, ///< Wait for any bit of the flags to be set
    K_FLAGS_SET_ALL =
        1 << 1u, ///< Wait for all bits of the flags to be set (not supported)
    K_FLAGS_CLR_ANY =
        1 << 2u, ///< Wait for any bit of the flags to be cleared (not supported)
    K_FLAGS_CLR_ALL =
        1 << 3u, ///< Wait for all bits of the flags to be cleared (not supported)
    K_FLAGS_CONSUME = 1 << 4u, ///< Consume the flags after notification

    /* Notification options */
    K_FLAGS_SET   = 1 << 5u, ///< Set the flags
    K_FLAGS_CLR   = 1 << 6u, ///< Clear the flags (not supported)
    K_FLAGS_SCHED = 1 << 7u, ///< Call the scheduler if a task was woken up
} k_flags_options_t;

/**
 * @brief Initialize a flags object at runtime.
 *
 * This function initializes a flags object with a specified initial value.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param flags Pointer to the flags object to initialize.
 * @param value Initial value of the flags.
 * @return 0 on success, or -EINVAL if the flags pointer is NULL.
 */
int8_t k_flags_init(struct k_flags *flags, k_flags_value_t value);

/**
 * @brief Poll for specified flags to be set or cleared.
 *
 * This function allows a thread to wait for specific flags to be set
 * according to the provided mask and options. If the flags are already
 * set as required, the function returns immediately. Otherwise, the
 * thread will block until the condition is met or the timeout expires.
 *
 * Safety: This function is generally not safe to call from an ISR context
 *         if the timeout is different from K_NO_WAIT.
 *
 * @param flags Pointer to the flags object.
 * @param mask Mask of bits to wait for.
 * @param options Polling options, e.g., K_FLAGS_SET_ANY and K_FLAGS_CONSUME.
 * @param timeout Timeout value to wait for the flags.
 * @return Positive value indicating the bits that caused the wake-up, or
 * a negative error code on failure.
 * @retval -EINVAL if the flags pointer is NULL.
 * @retval -ENOTSUP if the options are not supported.
 * @retval -EAGAIN if the timeout expires before the flags are set.
 */
int k_flags_poll(struct k_flags *flags,
                 k_flags_value_t *mask,
                 k_flags_options_t options,
                 k_timeout_t timeout);

/**
 * @brief Notify a flags object to set or clear specific bits.
 *
 * This function sets or clears bits in the flags object according to the
 * provided mask and options. It can optionally wake up threads waiting
 * on the flags and invoke the scheduler if required.
 *
 * @param flags Pointer to the flags object.
 * @param notify_value Value of the bits to set or clear.
 * @param options Notification options, e.g., K_FLAGS_SET and K_FLAGS_SCHED.
 * @return Number of threads that were woken up, or a negative error code on failure.
 * @retval -EINVAL if the flags pointer is NULL.
 * @retval -ENOTSUP if the options are not supported.
 *
 * Safety:  This function is generally safe to call from an ISR context but
 * K_FLAGS_SCHED should not be used. Instead, use the
 * return value to check if a task was woken up, and call k_yield_from_isr_cond()
 * if the return value is greater than 0.
 */
int8_t k_flags_notify(struct k_flags *flags,
                      k_flags_value_t notify_value,
                      k_flags_options_t options);

/**
 * @brief Reset a flags object to its initial state.
 *
 * This function resets the flags object to its initial value and cancels
 * all pending threads waiting on the flags.
 *
 * Safety: This function is safe to call from an ISR context.
 *
 * @param flags Pointer to the flags object.
 * @return 0 on success, or a negative error code on failure.
 */
int8_t k_flags_reset(struct k_flags *flags);

#endif /* _AVRTOS_FLAGS_H */
