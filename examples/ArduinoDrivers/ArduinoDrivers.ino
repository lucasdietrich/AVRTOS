/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos.h>
#include <avrtos/drivers/exti.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/timer.h>
#include <avrtos/drivers/usart.h>

#if defined(__AVR_ATmega2560__)
#define INT0_PORT GPIOD
#define INT0_PIN  0u
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
#define INT0_PORT GPIOD
#define INT0_PIN  2u
#else
#error "Unsupported MCU for this example"
#endif

ISR(TIMER1_COMPA_vect)
{
    static uint8_t i = 0u;

    /* Toggle builtin LED */
    digitalWrite(LED_BUILTIN, i = !i);
}

ISR(INT0_vect)
{
    /* Send a character to USART0 */
    ll_usart_sync_putc(USART0_DEVICE, '!');
}

ISR(USART0_RX_vect)
{
    /* Read data register to clear interrupt flag */
    (void)USART0_DEVICE->UDRn;

    /* Generate a software interrupt */
    gpiol_pin_toggle(INT0_PORT, INT0_PIN);
}

void setup(void)
{
    /* LED initialisation */
    pinMode(LED_BUILTIN, OUTPUT);

    /* UART initialisation */
    const struct usart_config usart_config = {
        .baudrate    = CONFIG_SERIAL_USART_BAUDRATE,
        .receiver    = 1u,
        .transmitter = 1u,
        .mode        = USART_MODE_ASYNCHRONOUS,
        .parity      = USART_PARITY_NONE,
        .stopbits    = USART_STOP_BITS_1,
        .databits    = USART_DATA_BITS_8,
        .speed_mode  = USART_SPEED_MODE_NORMAL,
    };
    usart_init(USART0_DEVICE, &usart_config);

    /* Enable USART RX interrupt */
    ll_usart_enable_rx_isr(USART0_DEVICE);

    /* Timer initialisation */
    const struct timer_config timer_config {
        .mode = TIMER_MODE_CTC, .prescaler = TIMER_PRESCALER_1024,
        .counter = TIMER_CALC_COUNTER_VALUE(1000000lu, 1024lu), .timsk = BIT(OCIEnA),
    };
    timer16_init(TIMER1_DEVICE, &timer_config);

    /* Configure INT0 pin as output to generate a software interrupt */
    gpiol_pin_init(INT0_PORT, INT0_PIN, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);

    /* Configure INT0 interrupt */
    exti_configure(INT0, ISC_EDGE);
    exti_clear_flag(INT0);
    exti_enable(INT0);

    printf_P(PSTR("Send a character to USART0 to trigger a software interrupt\n"));
}

void loop(void)
{
    // nothing to do, everything is handled by interrupts
    // ...
}