/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

void task_handler(struct k_work *self)
{
	static uint8_t counter = 0;

	/* Sleeping in system workqueue.
	 * Not recommended because it would block any other work
	 * items sent to the system workqueue, but it's possible.
	 */
	k_sleep(K_SECONDS(1));

	usart_print_p(PSTR("Hello from system workqueue, counter value = "));
	usart_u8(counter++);
	usart_transmit('\n');

	/* Work item submits itself.
	 * Work item is now submittable because it started being.
	 */
	k_system_workqueue_submit(self);
}

K_WORK_DEFINE(work, task_handler);

/*___________________________________________________________________________*/

int main(void)
{
	led_init();
	usart_init();

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

/*___________________________________________________________________________*/