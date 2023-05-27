/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/kernel.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

void task_handler(struct k_work *self)
{
	static uint8_t counter = 0;

	/* Sleeping in system workqueue.
	 * Not recommended because it would block any other work
	 * items sent to the system workqueue, but it's possible.
	 */
	k_sleep(K_SECONDS(1));

	serial_print_p(PSTR("Hello from system workqueue, counter value = "));
	serial_u8(counter++);
	serial_transmit('\n');

	/* Work item submits itself.
	 * Work item is now submittable because its handler already
	 * started being executed.
	 */
	k_system_workqueue_submit(self);
}

K_WORK_DEFINE(work, task_handler);

int main(void)
{
	led_init();
	serial_init();

	k_thread_dump_all();

	irq_enable();

	K_SCHED_LOCK_CONTEXT
	{
		/* schedule the work item */
		k_system_workqueue_submit(&work);

		/* scheduling the work item before it
		 * started being process have no effect
		 */
		k_system_workqueue_submit(&work);
	}

	for (;;) {
		/* some processing */
	}
}