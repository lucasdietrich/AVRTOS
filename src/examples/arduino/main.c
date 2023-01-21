/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/kernel.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>

uint8_t task_stack[512u];
struct k_thread task;
struct k_timer tm;
struct k_sem sem;

static void t1_entry(void *context)
{
	(void)context;

	for (;;) {
		k_sem_take(&sem, K_FOREVER);
		printf_P(PSTR("Hello World from task!\n"));
	}
}

void tm_cb(struct k_timer *timer)
{
	k_sem_give(&sem);
}

int main(void)
{
	z_avrtos_init();

	serial_init();

	k_sem_init(&sem, 0u, 1u);

	k_thread_create(
		&task, t1_entry, task_stack, sizeof(task_stack), K_PREEMPTIVE, NULL, 'T');
	k_thread_start(&task);

	k_timer_init(&tm, tm_cb, K_SECONDS(2u), K_NO_WAIT);

	for (;;) {
		printf_P(PSTR("Hello World from main!\n"));
		k_sleep(K_SECONDS(1u));
	}
}