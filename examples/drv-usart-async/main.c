/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// For ATmega328PB or ATmega2560

#include <avrtos/debug.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/avrtos.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#define K_MODULE K_MODULE_APPLICATION

void consumer(void *context);

K_THREAD_DEFINE(w1, consumer, 0x100, K_PREEMPTIVE, NULL, 'A');

struct in {
	struct snode tie;
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

void free_mem(struct in *mem)
{
	k_mem_slab_free(&myslab, mem);
}

__always_inline void input(const char rx)
{
	static struct in *mem = NULL;
	if (mem == NULL) {
		if (alloc(&mem) != 0) {
			__ASSERT_NULL(mem);
			serial_transmit('!');
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
			serial_transmit(rx);
		}
		break;
	default:
		if (mem->len == sizeof(mem->buffer) - 1u) {
			mem->len = 0;
			push(&mem);
		} else {
			mem->buffer[mem->len++] = rx;
		}
		serial_transmit(rx);
		break;
	}
}

ISR(USART0_RX_vect)
{
	const char rx = UDR0;
	input(rx);
}

void consumer(void *context)
{
	for (;;) {
		serial_print_p(PSTR("\n# "));
		struct in *mem = (struct in *)k_fifo_get(&myfifo, K_FOREVER);
		__ASSERT_NOTNULL(mem);
		if (mem->len == 0) {
			serial_print_p(PSTR("\nCOMMAND DROPPED !"));
		} else {
			/* process/parsed the command */
			serial_print_p(PSTR("CMD received ! len = "));
			serial_u8(mem->len);
			serial_print_p(PSTR(" : "));

			for (uint8_t *c = (uint8_t *)mem->buffer;
			     c < mem->buffer + mem->len;
			     c++) {
				serial_transmit(*c);
			}
		}
		k_mem_slab_free(&myslab, mem);
	}
}

// IPC uart
const struct usart_config usart_ipc_cfg PROGMEM = {.baudrate	= USART_BAUD_1000000,
						   .receiver	= 1,
						   .transmitter = 1,
						   .mode	= USART_MODE_ASYNCHRONOUS,
						   .parity	= USART_PARITY_NONE,
						   .stopbits	= USART_STOP_BITS_1,
						   .databits	= USART_DATA_BITS_8,
						   .speed_mode = USART_SPEED_MODE_NORMAL};

#define BUFFER_SIZE 16

static uint8_t rx_buffer[BUFFER_SIZE];

K_MSGQ_DEFINE(ipc_msgq, BUFFER_SIZE, 2);

const uint8_t tx_buffer[] = "Hello World !";

void work_tx(struct k_work *w)
{
	k_sleep(K_SECONDS(1));
	usart_tx(USART1_DEVICE, tx_buffer, sizeof(tx_buffer) - 1);
}

K_WORK_DEFINE(tx_work, work_tx);

void usart_ipc_callback(UART_Device *dev, struct usart_async_context *ctx)
{
	if (ctx->evt == USART_EVENT_RX_COMPLETE) {
		k_msgq_put(&ipc_msgq, ctx->rx.buf, K_NO_WAIT);
	} else if (ctx->evt == USART_EVENT_TX_COMPLETE) {
		k_system_workqueue_submit(&tx_work);
	}
}

int main(void)
{
	irq_enable();

	// initialize shell uart
	serial_init();
	SET_BIT(UCSR0B, 1 << RXCIE0); // enable RX interrupt for shell uart

	// initialize IPC uart
	struct usart_config cfg;
	memcpy_P(&cfg, &usart_ipc_cfg, sizeof(struct usart_config));
	usart_init(USART1_DEVICE, &cfg);

	usart_set_callback(USART1_DEVICE, usart_ipc_callback);
	usart_rx_enable(USART1_DEVICE, rx_buffer, sizeof(rx_buffer));

	usart_tx(USART1_DEVICE, tx_buffer, sizeof(tx_buffer) - 1);

	k_sleep(K_FOREVER);
}

static void usart_rx_thread(struct k_msgq *msgq)
{
	static uint8_t data[BUFFER_SIZE];

	for (;;) {
		k_msgq_get(msgq, data, K_FOREVER);

		// print data size
		for (uint8_t *c = data; c < data + BUFFER_SIZE; c++) {
			printf_P(PSTR(" %c"), *c);
		}
		printf_P(PSTR("\n"));
	}
}

K_THREAD_DEFINE(rx_thread, usart_rx_thread, 0x100, K_COOPERATIVE, &ipc_msgq, 'X');

static void thread_canaries(void *arg)
{
	for (;;) {
		k_dump_stack_canaries();
		k_sleep(K_SECONDS(30));
	}
}

K_THREAD_DEFINE(canary, thread_canaries, 0x100, K_COOPERATIVE, NULL, 'C');