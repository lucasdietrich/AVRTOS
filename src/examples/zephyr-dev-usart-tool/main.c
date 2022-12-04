/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/serial.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>
#include <avrtos/drivers/usart.h>

#define K_MODULE K_MODULE_APPLICATION

#define IPC_MAX_DATA_SIZE 0x10U

#define IPC_START_FRAME_DELIMITER ((uint32_t) 0xAAAAAAAALU)
#define IPC_START_FRAME_DELIMITER_SIZE 4U
#define IPC_END_FRAME_DELIMITER ((uint32_t) 0x55555555LU)
#define IPC_END_FRAME_DELIMITER_SIZE 4U

#define IPC_FRAME_SIZE ((size_t) sizeof(ipc_frame_t))

typedef struct {
	uint16_t size;
	uint8_t buf[IPC_MAX_DATA_SIZE];
} __attribute__((packed)) ipc_data_t;

typedef struct {
	uint32_t start_delimiter;
	uint32_t seq;
	ipc_data_t data;
	uint32_t crc32;
	uint32_t end_delimiter;
} __attribute__((packed)) ipc_frame_t;

const struct usart_config usart_ipc_cfg = {
	.baudrate = USART_BAUD_500000,
	.receiver = 1,
	.transmitter = 1,
	.mode = USART_MODE_ASYNCHRONOUS,
	.parity = USART_PARITY_NONE,
	.stopbits = USART_STOP_BITS_1,
	.databits = USART_DATA_BITS_8,
	.speed_mode = USART_SPEED_MODE_NORMAL
};

uint8_t rx_buffer[IPC_FRAME_SIZE];

static uint8_t msgq_buffer[2][IPC_FRAME_SIZE];
K_MSGQ_DEFINE(ipc_msgq, msgq_buffer, IPC_FRAME_SIZE, 2LU);

K_SEM_DEFINE(tx_finished_sem, 1, 1);

void usart_ipc_callback(UART_Device *dev, struct usart_async_context *ctx)
{
	if (ctx->evt == USART_EVENT_RX_COMPLETE) {
		k_msgq_put(&ipc_msgq, ctx->rx.buf, K_NO_WAIT);
	} else if (ctx->evt == USART_EVENT_TX_COMPLETE) {
		k_sem_give(&tx_finished_sem);
	}
}

static ipc_frame_t tx_frame;

K_PRNG_DEFINE_DEFAULT(prng);

static void build_tx_frame(ipc_frame_t *frame)
{
	frame->start_delimiter = IPC_START_FRAME_DELIMITER;
	frame->seq = 0U;
	frame->data.size = IPC_MAX_DATA_SIZE;
	frame->crc32 = 0xBBBBBBBBLU;
	frame->end_delimiter = IPC_END_FRAME_DELIMITER;

	for (uint32_t i = 0; i < IPC_MAX_DATA_SIZE; i++) {
		frame->data.buf[i] = i;
	}

	// tx_frame.data.size = IPC_MAX_DATA_SIZE;
	// k_prng_get_buffer(&prng, (uint8_t *)&tx_frame.data.buf, IPC_MAX_DATA_SIZE);
}

int main(void)
{
	irq_enable();

	serial_init();

	usart_init(USART1_DEVICE, &usart_ipc_cfg);
	usart_set_callback(USART1_DEVICE, usart_ipc_callback);

	// usart_rx_enable(USART1_DEVICE, rx_buffer, sizeof(rx_buffer));

	build_tx_frame(&tx_frame);

	for (uint32_t i = 0;; i++) {
		k_sem_take(&tx_finished_sem, K_FOREVER);

		size_t len = sizeof(tx_frame);
		// if ((i % 10) == 0)
		// 	len -= 1;

		printf_P(PSTR("TX len : %u\n"), len);

		tx_frame.seq++;
		usart_tx(USART1_DEVICE, &tx_frame, len);

		k_sleep(K_MSEC(5000));
	}
}


static void usart_rx_thread(struct k_msgq *msgq)
{
	static uint8_t buffer[IPC_FRAME_SIZE];

	for (;;) {
		k_msgq_get(msgq, &buffer, K_FOREVER);

		// print buffer size
		printf_P(PSTR("Buffer size : %u\n"), IPC_FRAME_SIZE);

		// Pretty print buffer
		for (int i = 0; i < IPC_FRAME_SIZE; i++) {
			if (i % 16 == 0) {
				printf_P(PSTR("\n"));
			}
			printf_P(PSTR("%02x "), buffer[i]);
		}

		printf_P(PSTR("\n"));
	}
}

K_THREAD_DEFINE(rx_thread, usart_rx_thread, 0x100, K_COOPERATIVE, &ipc_msgq, 'X');

static void thread_canaries(void *arg)
{
	for (;;) {
		dump_stack_canaries();
		k_sleep(K_SECONDS(30));
	}
}

K_THREAD_DEFINE(canary, thread_canaries, 0x100, K_COOPERATIVE, NULL, 'C');