#include <avrtos/kernel.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/exti.h>

ISR(INT0_vect) {
	/* Generate signal on measurement line */
	gpio_toggle_pin(GPIOB_DEVICE, PIN7);

	/* Generate a soft int on INT1 */
	gpio_toggle_pin(GPIOD_DEVICE, PIN1);
}

ISR(INT1_vect) {
	/* Generate signal on measurement line */
	gpio_toggle_pin(GPIOB_DEVICE, PIN6);
}

int main(void)
{
	/* Init INT gpios */
	gpio_init(GPIOB_DEVICE, DIR_OUTPUT, OUTPUT_DRIVEN_LOW);

	/* Init measurements gpios */
	gpio_init(GPIOD_DEVICE, DIR_OUTPUT, OUTPUT_DRIVEN_LOW);
	gpio_set_pin_direction(GPIOD_DEVICE, PIN0, DIR_OUTPUT);
	gpio_set_pin_output_state(GPIOD_DEVICE, PIN0, STATE_LOW);
	gpio_set_pin_direction(GPIOD_DEVICE, PIN1, DIR_OUTPUT);
	gpio_set_pin_output_state(GPIOD_DEVICE, PIN1, STATE_LOW);

	/* Unecessary, just to have clean signals on my lines */
	gpio_init(GPIOH_DEVICE, DIR_OUTPUT, OUTPUT_DRIVEN_LOW);

	/* Configure INT0 */
	exti_configure(INT0, ISC_EDGE);
	exti_clear_flag(INT0);
	exti_enable(INT0);

	/* Configure INT1 */
	exti_configure(INT1, ISC_EDGE);
	exti_clear_flag(INT1);
	exti_enable(INT1);

	/* Enable interrupts */
	irq_enable();

	for (;;) {
		/* Generate a soft int on INT0 */
		k_sleep(K_MSEC(1u));
		gpio_toggle_pin(GPIOD_DEVICE, PIN0);
	}
}