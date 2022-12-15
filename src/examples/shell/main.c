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
	struct k_thread *thread = k_fifo_put(&myfifo, *(void **)mem);
	*mem = NULL;
	k_yield_from_isr_cond(thread);
}

int8_t alloc_in(struct in **mem)
{
	return k_mem_slab_alloc(&myslab, (void **)mem, K_NO_WAIT);
}

void free_in(struct in *mem)
{
	k_mem_slab_free(&myslab, mem);
}

/*___________________________________________________________________________*/

static inline void input(const char rx)
{
	static struct in *mem = NULL;

	if (mem == NULL) {
		if (alloc_in(&mem) != 0) {
			__ASSERT_NULL(mem);
			serial_transmit('!');
			return;
		}
		mem->len = 0;
	}

	// serial_hex(rx);

	switch (rx) {
	case 0x1A: /* Ctrl + Z -> drop */
		mem->len = 0;
	case '\r':
	case '\n': /* process the packet */
		mem->buffer[mem->len] = '\0';
		push(&mem);
		break;
	case 0x7f:
	case '\b': /* backspace */
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

		if (mem->len != 0) {
			/* process/parsed the command */
			serial_print_p(PSTR("\nlen="));
			serial_u8(mem->len);
			serial_print_p(PSTR(" : "));

			for (uint8_t *c = (uint8_t *)mem->buffer;
			     c < mem->buffer + mem->len; c++) {
				serial_transmit(*c);
			}
		}
		free_in(mem);
	}
}

int main(void)
{
	serial_init();

	k_thread_dump_all();

	SET_BIT(UCSR0B, 1 << RXCIE0);

	k_sleep(K_FOREVER);
}