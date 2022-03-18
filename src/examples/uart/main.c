/**
 * @file main.c
 * @author Dietrich Lucas (ld.adecy@gmail.com)
 * @brief ATmega2560 example, connect RX1 to TX1 using a wire
 * @version 0.1
 * @date 2022-03-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>
#include <avrtos/drivers/usart.h>

/*___________________________________________________________________________*/

#define K_MODULE K_MODULE_APPLICATION

/*___________________________________________________________________________*/

void consumer(void *context);

K_THREAD_DEFINE(w1, consumer, 0x100, K_PREEMPTIVE, NULL, 'A');

struct in
{
        struct qitem tie;
        uint8_t buffer[20];
        uint8_t len;
};

K_MEM_SLAB_DEFINE(myslab, sizeof(struct in), 2u);
K_FIFO_DEFINE(myfifo);

void push(struct in **mem)
{
        k_fifo_put(&myfifo, *(void **)mem);
        *mem = NULL;
}

int8_t alloc(struct in **mem)
{
        return k_mem_slab_alloc(&myslab, (void **)mem, K_NO_WAIT);
}

void free(struct in *mem)
{
        k_mem_slab_free(&myslab, mem);
}

/*___________________________________________________________________________*/

static inline void input(const char rx)
{
        static struct in *mem = NULL;
        if (mem == NULL) {
                if (alloc(&mem) != 0) {
                        __ASSERT_NULL(mem);
                        usart_transmit('!');
                        return;
                }
                mem->len = 0;
        }

        switch (rx) {
        case 0x1A: /* Ctrl + Z -> drop */
                mem->len = 0;
        case '\n': /* process the packet */
                mem->buffer[mem->len] = '\0';
                push(&mem);
                break;
        case 0x08: /* backspace */
                if (mem->len > 0) {
                        mem->len--;
                        usart_transmit(rx);
                }
                break;
        default:
                if (mem->len == sizeof(mem->buffer) - 1u) {
                        mem->len = 0;
                        push(&mem);
                } else {
                        mem->buffer[mem->len++] = rx;
                }
                usart_transmit(rx);
                break;
        }
}

#if defined(__AVR_ATmega328P__)
#	define board_USART_RX_vect  USART_RX_vect
#elif defined(__AVR_ATmega2560__)
#	define board_USART_RX_vect  USART0_RX_vect
#endif /* __AVR_ATmega328P__ */

ISR(board_USART_RX_vect)
{
        const char rx = UDR0;
        input(rx);
}

void consumer(void *context)
{
        for (;;) {
                usart_print_p(PSTR("\n# "));
                struct in *mem = (struct in *)k_fifo_get(&myfifo, K_FOREVER);
                __ASSERT_NOTNULL(mem);
                if (mem->len == 0) {
                        usart_print_p(PSTR("\nCOMMAND DROPPED !"));
                } else {
                        /* process/parsed the command */
                        usart_print_p(PSTR("CMD received ! len = "));
                        usart_u8(mem->len);
			usart_print_p(PSTR(" : "));

			for (uint8_t *c = (uint8_t *)mem->buffer;
			     c < mem->buffer + mem->len; c++) {
				usart_transmit(*c);
			}
                }
                k_mem_slab_free(&myslab, mem);
        }
}

/*___________________________________________________________________________*/

// IPC uart
const struct usart_config usart_ipc_cfg PROGMEM = {
	.baudrate = USART_BAUD_1000000,
	.receiver = 1,
	.transmitter = 1,
	.mode = USART_MODE_ASYNCHRONOUS,
	.parity = USART_PARITY_NONE,
	.stopbits = USART_STOP_BITS_1,
	.databits = USART_DATA_BITS_8,
	.speed_mode = USART_SPEED_MODE_NORMAL
};

#if DRIVERS_UART_ASYNC == 0

ISR(USART1_RX_vect)
{
        usart_transmit(UDR1);
}

#endif 

#define BUFFER_SIZE 16

static uint8_t rx_buffer[BUFFER_SIZE];

static uint8_t msgq_buffer[2][BUFFER_SIZE];
K_MSGQ_DEFINE(ipc_msgq, msgq_buffer, BUFFER_SIZE, 2);

void usart_ipc_rx_callback(UART_Device *dev, struct usart_async_context *ctx)
{
	k_msgq_put(&ipc_msgq, ctx->buf.data, K_NO_WAIT);
}

int main(void)
{
	irq_enable();

	// initialize shell uart
        usart_init();
	SET_BIT(UCSR0B, 1 << RXCIE0); // enable RX interrupt for shell uart

	// initialize IPC uart
	struct usart_config cfg;
	memcpy_P(&cfg, &usart_ipc_cfg, sizeof(struct usart_config));
	usart_drv_init(UART1_DEVICE, &cfg);

	usart_set_callback(UART1_DEVICE, usart_ipc_rx_callback);
	usart_rx_enable(UART1_DEVICE, rx_buffer, sizeof(rx_buffer));

	// enable RX interrupt for IPC uart
	SET_BIT(UCSR1B, 1 << RXCIE1);

	uint8_t chr = 'a';

        for (;;) {
		usart_drv_sync_putc(UART1_DEVICE, chr);

		if (chr == 'z') {
			chr = 'a';
		} else {
			chr++;
		}

		k_sleep(K_MSEC(100));
	}
}


static void usart_rx_thread(struct k_msgq *msgq)
{
	static uint8_t data[BUFFER_SIZE];

	for (;;) {
		k_msgq_get(msgq, data, K_FOREVER);

		// print data size
		for (uint8_t *c = data; c < data + BUFFER_SIZE; c++) {
			printf_P(PSTR(" %x"), *c);
		}
		printf_P(PSTR("\n"));
	}
}

K_THREAD_DEFINE(rx_thread, usart_rx_thread, 0x100, K_COOPERATIVE, &ipc_msgq, 'X');

static void thread_canaries(void *arg)
{
	for (;;) {
		dump_stack_canaries();
		k_sleep(K_SECONDS(15));
	}
}

K_THREAD_DEFINE(canary, thread_canaries, 0x100, K_COOPERATIVE, NULL, 'C');