/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/stats.h>
#include <avrtos/avrtos.h>
#include <avrtos/misc/serial.h>

K_SEM_DEFINE(sem, 0, 1);

int main(void)
{
	serial_init();

	for (;;) {
		k_sem_give(&sem);

		k_msleep(1000u);
	}
}

void task_sem(void *arg)
{
	while (1) {
		k_sem_take(&sem, K_FOREVER);

		k_msleep(100u);
	}
}

void task_mon(void *arg)
{
	for (;;) {
		k_sleep(K_SECONDS(1));
	}
}

K_THREAD_DEFINE(thread_sem, task_sem, 0x80, K_PREEMPTIVE, NULL, 's');
K_THREAD_DEFINE(thread_mon, task_mon, 0x80, K_PREEMPTIVE, NULL, 'm');