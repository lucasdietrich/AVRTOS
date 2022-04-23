#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#include <avrtos/drivers/timer.h>
ISR(TIMER1_COMPA_vect)
{
	usart_transmit('*');
}

int main(void)
{
	led_init();
	usart_init();

	struct timer_config cfg = {
		.mode = TIMER_MODE_CTC,
		.prescaler = TIMER_PRESCALER_256,
		.counter = 62500U
	};

	ll_timer16_drv_init(TIMER1_DEVICE, &cfg);
	TIMER_TIMSK_SET_OCIEA(1U);

	k_stop();
}