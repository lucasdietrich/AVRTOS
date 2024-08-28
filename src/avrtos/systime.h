/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Uptime API and System Time Management
 *
 * This header file provides functions for retrieving the system uptime and
 * getting retrieving and setting the current system time (e.g. UTC or local time)
 * within the AVRTOS kernel.
 * It supports operations for accessing the current time, checking if the time is set.
 *
 * Related configuration options:
 * - CONFIG_KERNEL_UPTIME: Uptime feature must be enabled to use the time API.
 * - CONFIG_KERNEL_TIME_API: Time API must be enabled to use the system time functions.
 * - CONFIG_KERNEL_TIME_API_MS_PRECISION: Set to 1 to enable millisecond precision in the
 * time API, otherwise the time will be truncated to seconds.
 * - CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS: Set to 1 to enable 40-bit tick counter.
 */

#ifndef _AVRTOS_TIME_H_
#define _AVRTOS_TIME_H_

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Uptime API
 */

/**
 * @brief Get the current system uptime in 32-bit ticks.
 *
 * This function returns the uptime in kernel ticks, represented as a 32-bit
 * value.
 *
 * @return Kernel ticks value (32-bit).
 */
__kernel uint32_t k_ticks_get_32(void);

/**
 * @brief Get the current system uptime in ticks.
 *
 * This inline function is a wrapper for `k_ticks_get_32` and returns the
 * uptime in kernel ticks.
 *
 * @see k_ticks_get_32
 * @return Kernel ticks value (32-bit).
 */
__always_inline uint32_t k_ticks_get(void)
{
	return k_ticks_get_32();
}

/**
 * @brief Get the current system uptime in 64-bit ticks.
 *
 * This function returns the uptime in kernel ticks, represented as a 64-bit
 * value. It is meaningful only if `CONFIG_KERNEL_TICKS_COUNTER_40BITS` is
 * enabled in the configuration; otherwise, it behaves identically to
 * `k_ticks_get_32`.
 *
 * @return Kernel ticks value (64-bit).
 */
__kernel uint64_t k_ticks_get_64(void);

/**
 * @brief Get the current system uptime in milliseconds (32-bit).
 *
 * This function returns the uptime in milliseconds as a 32-bit value, and is
 * meaningful only if `CONFIG_KERNEL_UPTIME` is enabled in the configuration.
 *
 * @return Uptime in milliseconds (32-bit).
 */
__kernel uint32_t k_uptime_get_ms32(void);

/**
 * @brief Get the current system uptime in milliseconds (64-bit).
 *
 * This function returns the uptime in milliseconds as a 64-bit value. It should
 * be used if `CONFIG_CONFIG_KERNEL_TICKS_COUNTER_40BITS` is enabled in the
 * configuration. Otherwise, use `k_uptime_get_ms32`.
 *
 * @return Uptime in milliseconds (64-bit).
 */
__kernel uint64_t k_uptime_get_ms64(void);

/**
 * @brief Get the current system uptime in seconds.
 *
 * This function returns the uptime in seconds. It is meaningful only if
 * `KERNEL_UPTIME` is enabled in the configuration.
 *
 * @return Uptime in seconds.
 */
__kernel uint32_t k_uptime_get(void);

/**
 * @brief Print the current uptime in seconds to the serial output.
 *
 * This function prints the uptime of the system in seconds.
 */
void k_show_uptime(void);

/**
 * @brief Print the current uptime in ticks to the serial output.
 *
 * This function prints the uptime of the system in ticks.
 */
void k_show_ticks(void);

/**
 * @brief Structure to represent time in seconds and milliseconds.
 */
struct timespec {
	uint32_t tv_sec;  /**< Seconds component of the time. */
	uint16_t tv_msec; /**< Milliseconds component of the time. */
};

/**
 * @brief Retrieve the current uptime as a `timespec` structure.
 *
 * @param ts Pointer to a `timespec` structure where the current time will be stored.
 */
__kernel void k_uptime_as_timespec_get(struct timespec *ts);

/**
 * System Time Management
 */

/**
 * @brief Set the current time.
 *
 * This function sets the system time to the specified number of seconds since epoch.
 *
 * @param sec The number of seconds to set the system time to.
 */
__kernel void k_time_set(uint32_t sec);

/**
 * @brief Check if the system time is set.
 *
 * This function checks whether the system time has been initialized.
 *
 * @return `true` if the system time is set, `false` otherwise.
 */
__kernel bool k_time_is_set(void);

/**
 * @brief Get the current time in seconds.
 *
 * This function returns the current system time in seconds since epoch.
 *
 * @return The current time in seconds.
 */
__kernel uint32_t k_time_get(void);

/**
 * @brief Get the current time in milliseconds.
 *
 * This function returns the current system time in milliseconds. If
 * `CONFIG_KERNEL_TIME_API_MS_PRECISION` is set to 0, the return value
 * will be truncated to seconds.
 *
 * @return The current time in milliseconds.
 */
__kernel uint64_t k_time_get_ms(void);

/**
 * @brief Unset the current time.
 *
 * This function clears the system time, effectively unsetting it.
 */
__kernel void k_time_unset(void);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_TIME_H_ */
