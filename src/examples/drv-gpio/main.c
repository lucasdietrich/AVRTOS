#include <avrtos/kernel.h>
#include <avrtos/drivers/gpio.h>

#define PINn PIN7
#define GPIO GPIOB_DEVICE

int main(void)
{
	irq_enable();
	
	gpio_init(GPIO, OUTPUT, OUTPUT_DRIVEN_LOW);

	for (;;) {
		GPIO->PIN = BIT(PINn);
		_delay_us(10);
	}
}