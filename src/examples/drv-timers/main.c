#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#include <avrtos/drivers/timer.h>

volatile uint32_t cnt = 0;

ISR(TIMER0_COMPA_vect)
{
	cnt++;
	usart_transmit('0');
}

ISR(TIMER5_COMPA_vect)
{
	usart_transmit('5');
}

int main(void)
{
	led_init();
	usart_init();

	struct timer_config cfg = {
		.mode = TIMER_MODE_CTC,
		.prescaler = TIMER_PRESCALER_256,
		.counter = 62500U - 1U
	};

	ll_timer16_drv_init(TIMER5_DEVICE, &cfg);
	TIMER_TIMSK_SET_OCIEA(timer_get_index(TIMER5_DEVICE));

	cfg.prescaler = TIMER_PRESCALER_1024;
	cfg.counter = 255U;

	ll_timer8_drv_init(TIMER0_DEVICE, &cfg);
	TIMER_TIMSK_SET_OCIEA(0);
	
	for (;;) {
		printf("\n");
		k_show_uptime();

		k_wait(K_SECONDS(1));
	}
}