#include <avr/pgmspace.h>

#include <avrtos/kernel.h>
#include <avrtos/drivers/usart.h>

const struct usart_config pgm_cfg PROGMEM = USART_CONFIG_DEFAULT();

int main(void)
{
        int ret;
        struct usart_config cfg;
        memcpy_P(&cfg, &pgm_cfg, sizeof(struct usart_config));

        ret = usart_drv_init(0, &cfg);

        if (ret != 0) {
                for (;;) {
                        
                }
        }

        for (;;) {
                usart_drv_sync_putc(0, 'A');
                usart0_drv_sync_putc_opt('A');
                k_sleep(K_SECONDS(1));
        }
}