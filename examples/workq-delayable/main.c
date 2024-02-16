/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/misc/serial.h>

struct MyStruct {
	struct k_work_delayable work;
	struct k_sem sem;
};

static struct MyStruct my_struct;

void work_handler(struct k_work *work)
{
	struct MyStruct *my_struct = CONTAINER_OF(work, struct MyStruct, work);

	k_sem_give(&my_struct->sem);
}

static void usart_task(void *arg)
{
	struct MyStruct *ms = (struct MyStruct *)arg;

	int8_t ret;
	uint8_t c;
	for (;;) {
		c = ll_usart_sync_getc(USART0_DEVICE);
		if (c == 'c') {
			printf_P(PSTR("Cancelling scheduled work\n"));
			ret = k_work_delayable_cancel(&ms->work);
		} else {
			printf_P(PSTR("Scheduling work\n"));
			ret = k_system_work_delayable_schedule(&ms->work, K_MSEC(1000));
		}

		if (ret == 0) {
			printf_P(PSTR("OK\n"));
		} else {
			printf_P(PSTR("Error: %d\n"), ret);
		}
	}
}

K_THREAD_DEFINE(usart, usart_task, 0x200, K_PREEMPTIVE, &my_struct, 'u');

int main(void)
{
	serial_init();

	k_work_delayable_init(&my_struct.work, work_handler);
	k_sem_init(&my_struct.sem, 0, 1);

	for (;;) {
		k_sem_take(&my_struct.sem, K_FOREVER);
		printf_P(PSTR("Work done !\n"));
	}
}