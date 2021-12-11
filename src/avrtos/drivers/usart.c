#include "usart.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <avr/pgmspace.h>
#include <avr/io.h>

/* same for all USARTs */
#define TXENn TXEN0
#define RXENn RXEN0
#define UMSELn0 UMSEL00
#define UMSELn1 UMSEL01
#define UPMn0 UPM00
#define UPMn1 UPM01
#define USBSn USBS0
#define UCSZn0 UCSZ00
#define UCSZn1 UCSZ01
#define UCSZn2 UCSZ02
#define UDREn UDRE0

struct usart_reg
{
        volatile uint8_t * UCSRnA;
        volatile uint8_t * UCSRnB;
        volatile uint8_t * UCSRnC;
        volatile uint8_t * UBRRnL;
        volatile uint8_t * UBRRnH;
        volatile uint8_t * UDRn;
};

static const struct usart_reg PROGMEM uarts[ARCH_USART_COUNT] = {
        {
                .UCSRnA = &UCSR0A,
                .UCSRnB = &UCSR0B,
                .UCSRnC = &UCSR0C,
                .UBRRnL = &UBRR0L,
                .UBRRnH = &UBRR0H,
                .UDRn = &UDR0,
        },
#if ARCH_USART_COUNT > 1
        {
                .UCSRnA = &UCSR1A,
                .UCSRnB = &UCSR1B,
                .UCSRnC = &UCSR1C,
                .UBRRnL = &UBRR1L,
                .UBRRnH = &UBRR1H,
                .UDRn = &UDR1,
        },
#endif /* ARCH_USART_COUNT > 1 */
#if ARCH_USART_COUNT > 2
        {
                .UCSRnA = &UCSR2A,
                .UCSRnB = &UCSR2B,
                .UCSRnC = &UCSR2C,
                .UBRRnL = &UBRR2L,
                .UBRRnH = &UBRR2H,
                .UDRn = &UDR2,
        },
#endif /* ARCH_USART_COUNT > 2 */
#if ARCH_USART_COUNT > 3
        {
                .UCSRnA = &UCSR3A,
                .UCSRnB = &UCSR3B,
                .UCSRnC = &UCSR3C,
                .UBRRnL = &UBRR3L,
                .UBRRnH = &UBRR3H,
                .UDRn = &UDR3,
        },
#endif /* ARCH_USART_COUNT > 3 */
};

static inline uint8_t get_reg(volatile uint8_t *reg)
{
        return *reg;
}

static inline void set_reg(volatile uint8_t *reg, uint8_t value)
{
        *reg = value;
}

static inline uint8_t get_pgm_reg(volatile uint8_t *const *pgm_reg)
{
        volatile uint8_t *reg = (volatile uint8_t *)pgm_read_word(pgm_reg);

        return get_reg(reg);
}

static inline void set_pgm_reg(volatile uint8_t *const *pgm_reg, uint8_t value)
{
        volatile uint8_t *reg = (volatile uint8_t *)pgm_read_word(pgm_reg);

        set_reg(reg, value);
}

static const struct usart_reg *get_pgm_regs(uint8_t usart_id)
{
        if (usart_id >= ARCH_USART_COUNT)
                return NULL;

        return &uarts[usart_id];
}

static void set_ubrr(const struct usart_reg *regs, uint16_t ubrr)
{
        set_pgm_reg(&regs->UBRRnH, (uint8_t)(ubrr >> 8));
        set_pgm_reg(&regs->UBRRnL, (uint8_t)ubrr);
}


/* check U2Xn bit which allow to double the speed */
static uint16_t calculate_ubrr(uint32_t baudrate, bool speed_mode)
{
        /* TODO use switch case or array ? */
        if (speed_mode) {
                return USART_CALC_SPEED_MODE_UBRRn(baudrate);
        } else {
                return USART_CALC_UBRRn(baudrate);
        }
}

static void set_baudrate(const struct usart_reg *regs,
                         uint32_t baudrate,
                         bool speed_mode)
{
        uint16_t ubrr = calculate_ubrr(baudrate, speed_mode);

        set_ubrr(regs, ubrr);
};

int usart_ll_init(uint8_t usart_id,
                   const struct usart_config *config)
{
        const struct usart_reg *regs = get_pgm_regs(usart_id);
        if (regs == NULL) {
                return -EINVAL;
        }

        /* Only asynchrone mode is supported */
        if (config->mode != USART_MODE_ASYNCHRONOUS) {
                return -EINVAL;
        }

        /* Invalid parity mode (reserved) */
        if (config->parity == 1u) {
                return -EINVAL;
        }

        /* Invalid frame format (reserved) */
        if ((config->databits & 0x4u) && (config->databits != USART_DATABITS_9)) {
                return -EINVAL;
        }

        /* set baudrate */
        set_baudrate(regs, config->baudrate, config->speed_mode);

        /* enable receiver and transmitter */
        uint8_t ucsrnb = 0u;
        if (config->transmitter)
                SET_BIT(ucsrnb, BIT(TXENn));
        if (config->receiver)
                SET_BIT(ucsrnb, BIT(RXENn));
        set_pgm_reg(&regs->UCSRnB, ucsrnb);


        uint8_t ucsrc = 0u;
        /* set USART mode asynchrone */
        SET_BIT(ucsrc, (0 << UMSELn0) | (0 << UMSELn1));
        /* set parity mode */
        SET_BIT(ucsrc, config->parity << UPMn0);
        /* Set stop bit */
        SET_BIT(ucsrc, config->stopbits << USBSn);
        /* set frame format */
        SET_BIT(ucsrc, config->databits << UCSZn0);
        set_pgm_reg(&regs->UCSRnC, ucsrc);

        return 0;
}

int usart_ll_deinit(uint8_t usart_id)
{
        const struct usart_reg *regs = get_pgm_regs(usart_id);
        if (regs == NULL) {
                return -EINVAL;
        }

        set_pgm_reg(&regs->UCSRnA, 0);
        set_pgm_reg(&regs->UCSRnB, 0);
        set_pgm_reg(&regs->UCSRnC, 0);

        return 0;
}

/*
 * Problem is that execution time is much longer than the old "usart_transmit",
 * because of fetch IO address for the proper USART from flash.
 */

// 0000085a <usart_transmit>:
//  85a:	90 91 c0 00 	lds	r25, 0x00C0	; 0x8000c0 <__TEXT_REGION_LENGTH__+0x7000c0>
//  85e:	95 ff       	sbrs	r25, 5
//  860:	fc cf       	rjmp	.-8      	; 0x85a <usart_transmit>
//  862:	80 93 c6 00 	sts	0x00C6, r24	; 0x8000c6 <__TEXT_REGION_LENGTH__+0x7000c6>
//  866:	08 95       	ret

// 0000076e <usart_ll_sync_putc.constprop.6>:
//  76e:	2c e7       	ldi	r18, 0x7C	; 124
//  770:	33 e0       	ldi	r19, 0x03	; 3
//  772:	f9 01       	movw	r30, r18
//  774:	a5 91       	lpm	r26, Z+
//  776:	b4 91       	lpm	r27, Z
//  778:	8c 91       	ld	r24, X
//  77a:	85 ff       	sbrs	r24, 5
//  77c:	fa cf       	rjmp	.-12     	; 0x772 <usart_ll_sync_putc.constprop.6+0x4>
//  77e:	e6 e8       	ldi	r30, 0x86	; 134
//  780:	f3 e0       	ldi	r31, 0x03	; 3
//  782:	a5 91       	lpm	r26, Z+
//  784:	b4 91       	lpm	r27, Z
//  786:	81 e4       	ldi	r24, 0x41	; 65
//  788:	8c 93       	st	X, r24
//  78a:	90 e0       	ldi	r25, 0x00	; 0
//  78c:	80 e0       	ldi	r24, 0x00	; 0
//  78e:	08 95       	ret
int usart_ll_sync_putc(uint8_t usart_id, char c)
{
        const struct usart_reg *regs = get_pgm_regs(usart_id);
        if (regs == NULL) {
                return -EINVAL;
        }

        /* wait for empty transmit buffer */
        while (!(get_pgm_reg(&regs->UCSRnA) & BIT(UDREn)));

        /* put data into HW buffer, sends the data */
        set_pgm_reg(&regs->UDRn, c);

        return 0;
}

/* as fast as usart_transmit */
void usart0_ll_sync_putc_opt(char c)
{
        while(!(UCSR0A & BIT(UDRE0)));
        UDR0 = c;
}

/*___________________________________________________________________________*/

int usart_drv_init(struct usart *dev,
                   const struct usart_config *config)
{
        int ret;
        
        ret = usart_ll_init(dev->data.id, config);

        if (ret != 0) {
                goto exit;
        }

        k_msgq_init(&dev->data.rx_msgq, dev->data.rx.buf,
                    1u, USART_DRV_RX_BUF_SIZE);

        k_msgq_init(&dev->data.tx_msgq, dev->data.tx.buf,
                    1u, USART_DRV_TX_BUF_SIZE);

	return 0;
exit:
        return ret;
};

int usart_drv_put_c(struct usart *dev,
                   char c,
                   k_timeout_t timeout)
{
        int ret;

	if (!dev) {
		return -EINVAL;
	}

        ret = k_msgq_put(&dev->data.tx_msgq, &c, timeout);

	switch(dev->data.id) {
		case USART_0:
			UCSR0B |= BIT(UDRIE0);
			break;
		
		case USART_1:
			UCSR1B |= BIT(UDRIE1);
			break;

		case USART_2:
			UCSR2B |= BIT(UDRIE2);
			break;

		case USART_3:
			UCSR3B |= BIT(UDRIE3);
			break;

		default:
			return -EINVAL;
	}	

        return ret;
}

char usart_drv_get_c(struct usart *dev,
                    k_timeout_t timeout)
{
        char c;
        int ret;

        ret = k_msgq_get(&dev->data.rx_msgq, &c, timeout);

        if (ret == 0) {
                return c;
        }

        return -EAGAIN;
}

int usart_drv_put_buf(struct usart *dev,
                     const char *buf,
                     size_t size,
                     k_timeout_t timeout)
{
        int ret;

        for (const char *c = buf; c < buf + size; c++) {
                ret = k_msgq_put(&dev->data.tx_msgq, c, timeout);
                if (ret != 0) {
                        return ret;
                }
        }

        return ret;
}

int usart_drv_get_buf(struct usart *dev,
                     char *buf,
                     size_t size,
                     k_timeout_t timeout)
{
        int ret;

        for (char *c = buf; c < buf + size; c++) {
                ret = k_msgq_get(&dev->data.rx_msgq, c, timeout);
                if (ret != 0) {
                        return ret;
                }
        }

        return ret;
}


static const struct usart_api usart_std_api = {
        .init = usart_drv_init,
        .put_c = usart_drv_put_c,
        .get_c = usart_drv_get_c,
        .put_buf = usart_drv_put_buf,
        .get_buf = usart_drv_get_buf,
};


struct usart usart0 = {
        .api = &usart_std_api,
        .data = {
                .id = USART_0,
        }
};

struct usart usart1 = {
        .api = &usart_std_api,
        .data = {
                .id = USART_1,
        }
};

ISR(USART1_RX_vect)
{
	const uint8_t c = UDR1;
	k_msgq_put(&usart1.data.rx_msgq, &c, K_NO_WAIT);
}

ISR(USART1_UDRE_vect)
{
	char c;
	int8_t ret = k_msgq_get(&usart1.data.tx_msgq, &c, K_NO_WAIT);

	if (ret == 0) {
		UDR1 = c;
	} else {
		UCSR1B &= ~BIT(UDRIE1);
	}
}

struct usart usart2 = {
        .api = &usart_std_api,
        .data = {
                .id = USART_2,
        }
};

ISR(USART2_RX_vect)
{
	const uint8_t c = UDR2;
	k_msgq_put(&usart2.data.rx_msgq, &c, K_NO_WAIT);
}

ISR(USART2_UDRE_vect)
{
	char c;
	int8_t ret = k_msgq_get(&usart2.data.tx_msgq, &c, K_NO_WAIT);

	if (ret == 0) {
		UDR2 = c;
	} else {
		UCSR2B &= ~BIT(UDRIE2);
	}
}

struct usart usart3 = {
        .api = &usart_std_api,
        .data = {
                .id = USART_3,
        }
};

ISR(USART3_RX_vect)
{
	const uint8_t c = UDR3;
	k_msgq_put(&usart3.data.rx_msgq, &c, K_NO_WAIT);
}

ISR(USART3_UDRE_vect)
{
	char c;
	int8_t ret = k_msgq_get(&usart3.data.tx_msgq, &c, K_NO_WAIT);

	if (ret == 0) {
		UDR3 = c;
	} else {
		UCSR3B &= ~BIT(UDRIE3);
	}
}