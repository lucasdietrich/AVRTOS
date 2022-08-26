#include <avrtos/kernel.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/drivers/gpio.h>

#define PINn PIN7
#define GPIO GPIOB_DEVICE

static void thread(void *arg);

K_THREAD_DEFINE(t1, thread, 0x100, K_COOPERATIVE, NULL, '1');

int main(void)
{
	irq_disable();

	gpio_init(GPIO, OUTPUT, OUTPUT_DRIVEN_LOW);

	thread(NULL);
}

void thread(void *arg)
{
	for (;;) {
		/* Writing a logic one to PINxn toggles the value of PORTxn, 
		 * independent on the value of DDRxn. Note that the SBI
		 * instruction can be used to toggle one single bit in a port.
		 */
		GPIO->PIN = BIT(PINn);
		k_yield();
	}
}