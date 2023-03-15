/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "time.h"

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

	printf_P(PSTR("%02lu:%02hhu:%02hhu [%lu.%03u s] : "),
		 hours,
		 (uint8_t)(minutes % 60),
		 (uint8_t)(seconds % 60),
		 ts.tv_sec,
		 ts.tv_msec);
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

static struct {
	uint32_t timestamp;
	uint32_t uptime_sec;
	struct k_mutex mutex;
} time_ref = {
	.timestamp  = 0,
	.uptime_sec = 0,
	.mutex	    = K_MUTEX_INIT(time_ref.mutex),
};

void k_time_set(uint32_t sec)
{
	k_mutex_lock(&time_ref.mutex, K_FOREVER);
	time_ref.uptime_sec = k_uptime_get();
	time_ref.timestamp  = sec;
	k_mutex_unlock(&time_ref.mutex);
}

uint32_t k_time_get(void)
{
	uint32_t timestamp;
	k_mutex_lock(&time_ref.mutex, K_FOREVER);
	timestamp = k_uptime_get();
	k_mutex_unlock(&time_ref.mutex);
	return timestamp - time_ref.uptime_sec + time_ref.timestamp;
}

bool k_time_is_set(void)
{
	k_mutex_lock(&time_ref.mutex, K_FOREVER);
	bool isset = time_ref.timestamp != 0;
	k_mutex_unlock(&time_ref.mutex);
	return isset;
}

void k_time_unset(void)
{
	k_mutex_lock(&time_ref.mutex, K_FOREVER);
	time_ref.timestamp  = 0;
	time_ref.uptime_sec = 0;
	k_mutex_unlock(&time_ref.mutex);
}

#endif /* CONFIG_KERNEL_TIME */