#include <avr/pgmspace.h>

#include <avrtos/kernel.h>
#include <avrtos/drivers/usart.h>

#include <avrtos/misc/uart.h>

#if !defined(__AVR_ATmega2560__)
#error "This example is for ATmega2560 only!"
#endif

const struct usart_config usart_dbg_pgm_cfg PROGMEM = USART_CONFIG_DEFAULT();
const struct usart_config usart_ipc_pgm_cfg PROGMEM = USART_CONFIG_DEFAULT_2400();

void usart1_thread(void *);
void usart2_thread(void *);

K_THREAD_DEFINE(usart1_id, usart1_thread, 0x100, K_COOPERATIVE, NULL, '1');
K_THREAD_DEFINE(usart2_id, usart2_thread, 0x100, K_COOPERATIVE, NULL, '1');

K_SEM_DEFINE(drivers_init, 0, 2);

int main(void)
{
        int ret;
        struct usart_config cfg;

        memcpy_P(&cfg, &usart_dbg_pgm_cfg, sizeof(struct usart_config));
        ret = usart_drv_init(0, &cfg);

        memcpy_P(&cfg, &usart_ipc_pgm_cfg, sizeof(struct usart_config));
        ret = usart_drv_init(1, &cfg);
        ret = usart_drv_init(2, &cfg);

        k_sem_give(&drivers_init);
        k_sem_give(&drivers_init);

        for (;;) {
                // usart_drv_sync_putc(0, 'A');
                // usart0_drv_sync_putc_opt('A');
                k_sleep(K_SECONDS(1));
        }
}

/*___________________________________________________________________________*/

ISR(USART1_RX_vect)
{
        usart_transmit(UBRR1);
}

ISR(USART1_TX_vect)
{
       usart_transmit('T');
}

ISR(USART1_UDRE_vect)
{
       usart_transmit('0');

       UCSR1B &= ~(1 << UDRIE1);
}

void usart1_thread(void *ctx)
{
        k_sem_take(&drivers_init, K_FOREVER);
        PRINT_PROGMEM_STRING(t1ready, "usart1_thread starting ...\n");

        // enable all USART1 interrupts
        UCSR1B |= _BV(RXCIE1) | _BV(TXCIE1);

        for(;;) {
                cli();
                usart_drv_sync_putc(1, '#');
                UCSR1B |= _BV(UDRIE1);
                sei();

                k_sleep(K_SECONDS(1));
        }
}

/*___________________________________________________________________________*/

static char usart2_buf[0x10];
K_MSGQ_DEFINE(usart2_msgq, usart2_buf, 1u, sizeof(usart2_buf));

ISR(USART2_RX_vect)
{
        uint8_t c = UDR2;
        k_msgq_put(&usart2_msgq, (void *)&c, K_NO_WAIT);
}

void usart2_thread(void *ctx)
{
        k_sem_take(&drivers_init, K_FOREVER);
        PRINT_PROGMEM_STRING(t1ready, "usart2_thread starting ...\n");

        /* activate RX interrupt */
        UCSR2B |= _BV(RXCIE2);

        char c;

        for(;;) {
                if (k_msgq_get(&usart2_msgq, &c, K_FOREVER) == 0) {
                        usart_transmit(_current->symbol);
                        usart_print(" : RX : ");
                        usart_transmit(c);
                        usart_transmit('\n');
                } else {
                        PRINT_PROGMEM_STRING(msgqgetfail, "usart2_thread k_msgq_get failed\n");
                }
        }
}