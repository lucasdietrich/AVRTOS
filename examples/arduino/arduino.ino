/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include <avrtos.h>
#include <avrtos/kernel.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>

uint8_t task_stack[512u];
struct k_thread task;
struct k_timer tm;
struct k_sem sem;
K_MEM_SLAB_DEFINE(memslab, 10u, 3u);
K_MSGQ_DEFINE(msgq, 10u, 3u);

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

void setup(void)
{
	serial_init();

	k_sem_init(&sem, 0u, 1u);
	k_thread_create(
		&task, t1_entry, task_stack, sizeof(task_stack), K_PREEMPTIVE, NULL, 'T');
	k_thread_start(&task);

	k_timer_init(&tm, tm_cb, K_SECONDS(2u), K_NO_WAIT);

	/* Not used here, but to show how to initialize a memslab and msgq
	 * defined using K_MEM_SLAB_DEFINE and K_MSGQ_DEFINE macros */
	k_mem_slab_init(&memslab, memslab.buffer, memslab.block_size, memslab.count);
	k_msgq_init(&msgq, (char *)msgq.buf_start, msgq.msg_size, msgq.max_msgs);
}

void loop(void)
{
	printf_P(PSTR("Hello World from main!\n"));
	k_sleep(K_SECONDS(1u));
}

#if !defined(ARDUINO)
int main(void)
{
	z_avrtos_init();

	setup();

	for (;;) {
		loop();
	}
}
#endif