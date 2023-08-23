/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "time.h"

#include <avr/pgmspace.h>

#include "mutex.h"

#if CONFIG_KERNEL_UPTIME

void k_timespec_get(struct timespec *ts)
{
	if (ts == NULL) {
		return;
	}

	const uint64_t ms = k_uptime_get_ms64();

	ts->tv_sec  = ms / 1000;
	ts->tv_msec = ms % 1000;
}

void k_show_uptime(void)
{
	struct timespec ts;
	k_timespec_get(&ts);

	uint32_t seconds = ts.tv_sec;
	uint32_t minutes = seconds / 60;
	uint32_t hours	 = minutes / 60;

	printf_P(PSTR("%02lu:%02hhu:%02hhu [%lu.%03u s] : "), hours,
		 (uint8_t)(minutes % 60), (uint8_t)(seconds % 60), ts.tv_sec, ts.tv_msec);
}

void k_show_ticks(void)
{
	const uint64_t ticks = k_ticks_get_64();
	const uint32_t msb   = (uint32_t)(ticks >> 32llu);
	const uint32_t lsb   = (uint32_t)ticks;

	if (msb) {
		printf_P(PSTR("%lu%lu"), msb, lsb);
	} else {
		printf_P(PSTR("%lu"), lsb);
	}
}

#if CONFIG_KERNEL_TIME_API

#if CONFIG_KERNEL_TIME_API_MS_PRECISION
typedef uint64_t k_uptime_t; /* ms */
#else
typedef uint32_t k_uptime_t;		    /* s */
#endif

static struct {
	/* last set timestamp (seconds) */
	uint32_t timestamp;

	/* uptime the timestamp was set
	 * - in seconds if CONFIG_KERNEL_TIME_API_MS_PRECISION is not set
	 * - in milliseconds if CONFIG_KERNEL_TIME_API_MS_PRECISION is set
	 */
	k_uptime_t uptime;
	/* mutex to protect timestamp and uptime */
	struct k_mutex mutex;
} z_time_ref = {
	.timestamp = 0u,
	.uptime	   = 0u,
	.mutex	   = K_MUTEX_INIT(z_time_ref.mutex),
};

void k_time_set(uint32_t sec)
{
	k_mutex_lock(&z_time_ref.mutex, K_FOREVER);
#if CONFIG_KERNEL_TIME_API_MS_PRECISION
	z_time_ref.uptime = k_uptime_get_ms64(); /* ms */
#else
	z_time_ref.uptime = k_uptime_get(); /* s */
#endif
	z_time_ref.timestamp = sec;
	k_mutex_unlock(&z_time_ref.mutex);
}

#if CONFIG_KERNEL_TIME_API_MS_PRECISION
uint32_t k_time_get(void)
{
	return (uint32_t)(k_time_get_ms() / MSEC_PER_SEC);
}

uint64_t k_time_get_ms(void)
{
	uint64_t timestamp_ms;

	k_mutex_lock(&z_time_ref.mutex, K_FOREVER);
	timestamp_ms = k_uptime_get_ms64() - z_time_ref.uptime +
		       (k_uptime_t)z_time_ref.timestamp * MSEC_PER_SEC;
	k_mutex_unlock(&z_time_ref.mutex);

	return timestamp_ms;
}

#else

uint32_t k_time_get(void)
{
	uint32_t timestamp_s;
	k_mutex_lock(&z_time_ref.mutex, K_FOREVER);
	timestamp_s = k_uptime_get() - z_time_ref.uptime + z_time_ref.timestamp;
	k_mutex_unlock(&z_time_ref.mutex);

	return timestamp_s;
}

uint64_t k_time_get_ms(void)
{
	return (uint64_t)k_time_get() * MSEC_PER_SEC;
}
#endif

bool k_time_is_set(void)
{
	k_mutex_lock(&z_time_ref.mutex, K_FOREVER);
	bool isset = z_time_ref.timestamp != 0;
	k_mutex_unlock(&z_time_ref.mutex);
	return isset;
}

void k_time_unset(void)
{
	k_mutex_lock(&z_time_ref.mutex, K_FOREVER);
	z_time_ref.timestamp = 0;
	z_time_ref.uptime    = 0;
	k_mutex_unlock(&z_time_ref.mutex);
}

#endif

#endif /* CONFIG_KERNEL_UPTIME */