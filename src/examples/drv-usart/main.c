#include <avr/pgmspace.h>

#include <avrtos/kernel.h>
#include <avrtos/drivers/usart.h>

#include <avrtos/misc/uart.h>

/**
 * @brief Example compatible with ATmega2560, 
 * Simply connect two wires between RX1 (19) and TX2 (16) 
 * and between TX1 (18) and RX2 (17). 
 * Both threads will be able to communicate toghether.
 */
#if !defined(__AVR_ATmega2560__)
#error "This example is for ATmega2560 only!"
#endif

const struct usart_config usart_dbg_pgm_cfg PROGMEM = USART_CONFIG_DEFAULT();
const struct usart_config usart_ipc_pgm_cfg PROGMEM = USART_CONFIG_DEFAULT_2400();

void usart1_thread(void *);
void usart2_thread(void *);

K_THREAD_DEFINE(usart1_id, usart1_thread, 0x100, K_COOPERATIVE, NULL, '1');
// K_THREAD_DEFINE(usart2_id, usart2_thread, 0x100, K_COOPERATIVE, NULL, '2');

K_SEM_DEFINE(drivers_init, 0, 2);

int main(void)
{
        // int ret;
        struct usart_config cfg;

        memcpy_P(&cfg, &usart_dbg_pgm_cfg, sizeof(struct usart_config));
        NEW_usart_init(&usart0, &cfg);

        memcpy_P(&cfg, &usart_ipc_pgm_cfg, sizeof(struct usart_config));
        NEW_usart_init(&usart1, &cfg);
        NEW_usart_init(&usart2, &cfg);

        k_sem_give(&drivers_init);
        k_sem_give(&drivers_init);

        for (;;) {
                // usart_ll_sync_putc(0, 'A');
                // usart0_ll_sync_putc_opt('A');
                k_sleep(K_SECONDS(1));
        }
}

/*___________________________________________________________________________*/

void usart1_thread(void *ctx)
{
        k_sem_take(&drivers_init, K_FOREVER);
        PRINT_PROGMEM_STRING(t1ready, "usart1_thread starting ...\n");

        // enable all USART1 interrupts
        UCSR1B |= _BV(RXCIE1) | _BV(TXCIE1) | _BV(UDRIE1);
        for(;;) {
		// usart_put_c(&usart0, '#', K_FOREVER);
                usart_put_c(&usart1, '#', K_FOREVER);

                k_sleep(K_SECONDS(1));
        }
}

/*___________________________________________________________________________*/

void usart2_thread(void *ctx)
{
        k_sem_take(&drivers_init, K_FOREVER);
        PRINT_PROGMEM_STRING(t1ready, "usart2_thread starting ...\n");

        /* activate RX interrupt */
        UCSR1B |= _BV(RXCIE2) | _BV(TXCIE2) | _BV(UDRIE2);

        char c;

        for(;;) {
		c = usart_get_c(&usart2, K_FOREVER);
                
		usart_transmit(_current->symbol);
		usart_print(" : RX : ");
		usart_transmit(c);
		usart_transmit('\n');
        }
}