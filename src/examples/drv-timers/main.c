#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

#include <avrtos/drivers/timer.h>

#if defined(__AVR_ATmega328P__)
#	define LED_DDR DDRB
#	define LED_PORT PORTB
#	define LED_PIN 5U
#elif defined(__AVR_ATmega2560__)
#	define LED_DDR DDRB
#	define LED_PORT PORTB
#	define LED_PIN 7U
#else
#	error "Unsupported MCU"
#endif

/* Led blinking */
ISR(TIMER1_COMPA_vect)
{
	static uint8_t cnt = 0U;
	if (cnt++ & 1) {
		LED_PORT |= _BV(LED_PIN);
	} else {
		LED_PORT &= ~_BV(LED_PIN);
	}
	usart_transmit('1');
}

ISR(TIMER2_COMPA_vect)
{
	usart_transmit('2');
}

#if defined(__AVR_ATmega2560__)
ISR(TIMER4_OVF_vect)
{
	/* Set counter to a value to have a 250ms period after first overflow */
	ll_timer16_set_tcnt(TIMER4_DEVICE,
			    TIMER_GET_MAX_COUNTER(4) -
			    TIMER_CALC_COUNTER_VALUE(250U * USEC_PER_MSEC, 1024));
	usart_transmit('4');
}
#endif

int main(void)
{
	usart_init();

	/* Initialize LED */
	LED_DDR |= _BV(LED_PIN);
	LED_PORT &= ~_BV(LED_PIN);

	struct timer_config cfg = {
		.mode = TIMER_MODE_CTC,
		.prescaler = TIMER_PRESCALER_256,
		.counter = TIMER_CALC_COUNTER_VALUE(100000LU, 256U),
		.timsk = BIT(OCIEnA),
	};
	ll_timer16_drv_init(TIMER1_DEVICE, timer_get_index(TIMER1_DEVICE), &cfg);

	cfg.prescaler = 0; /* Delay the start of the timer 2 */
	cfg.counter = 0xFFU;

	ll_timer8_drv_init(TIMER2_DEVICE, timer_get_index(TIMER2_DEVICE), &cfg);

#if defined(__AVR_ATmega2560__)
	cfg.counter = 0U; /* First overflow will be after ~4seconds */
	cfg.mode = TIMER_MODE_NORMAL;
	cfg.prescaler = TIMER_PRESCALER_1024; /* Delay the start of the timer 2 */
	cfg.timsk = BIT(TOIEn);

	ll_timer16_drv_init(TIMER4_DEVICE, timer_get_index(TIMER4_DEVICE), &cfg);
#endif

	uint32_t i = 0;
	for (;;) {
		/* Start du timer 2 */
		if (i == 3) {
			ll_timer_start(TIMER2_DEVICE, TIMER2_PRESCALER_1024);
		}

		printf("\n");
		k_show_uptime();
		k_wait(K_SECONDS(1));
		i++;
	}
}