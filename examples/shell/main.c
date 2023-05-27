/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/debug.h>
#include <avrtos/kernel.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <avrtos/logging.h>
#define LOG_LEVEL LOG_LEVEL_WRN

#define K_MODULE K_MODULE_APPLICATION

void consumer(void *context);

K_THREAD_DEFINE(w1, consumer, 0x100, K_PREEMPTIVE, NULL, 'A');

struct in {
	struct snode tie;
	char buffer[20];
	uint8_t len;
};

K_MEM_SLAB_DEFINE(myslab, sizeof(struct in), 2u);
K_FIFO_DEFINE(myfifo);

void push(struct in **mem)
{
	struct k_thread *thread = k_fifo_put(&myfifo, *(void **)mem);
	*mem			= NULL;
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
		/* TODO: With QEMU, the \n is not received, so assume EOL on \r */
#if !defined(__QEMU__)
		break;
#endif
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

struct command {
	char name[10u];
	uint8_t name_len;
	void (*func)(void);
};

static void cmd_help(void);
static void cmd_version(void);
static void cmd_reboot(void);
static void cmd_sleep(void);

#define CMD(_name, _func)                                                                \
	{                                                                                \
		.name = _name, .name_len = sizeof(_name) - 1u, .func = _func             \
	}

const struct command commands[] PROGMEM = {
	CMD("help", cmd_help),
	CMD("version", cmd_version),
	CMD("reboot", cmd_reboot),
	CMD("uptime", k_show_uptime),
	CMD("ticks", k_show_ticks),
	CMD("sleep", cmd_sleep),
	CMD("canaries", k_dump_stack_canaries),
	CMD("threads", k_thread_dump_all),
	CMD("led", led_toggle),
};

const struct command *find_command(const char *name)
{
	for (uint8_t i = 0; i < ARRAY_SIZE(commands); i++) {
		const struct command *cmd = &commands[i];
		if (strncmp_P(name, cmd->name, pgm_read_byte(&cmd->name_len)) == 0) {
			return cmd;
		}
	}

	return find_command("help");
}

void cmd_call(const struct command *cmd)
{
	void (*func)(void) = (void (*)(void))pgm_read_word(&cmd->func);
	func();
}

static void cmd_help(void)
{
	const struct command *cmd;

	for (uint8_t i = 0; i < ARRAY_SIZE(commands); i++) {
		cmd = &commands[i];
		printf_P(PSTR("\t"));
		printf_P(cmd->name);
		printf_P(PSTR("\n"));
	};
}

static void cmd_version(void)
{
	printf_P(PSTR("version: %02u.%02u.%02u"),
		 AVRTOS_VERSION_MAJOR,
		 AVRTOS_VERSION_MINOR,
		 AVRTOS_VERSION_REVISION);
}

static void cmd_reboot(void)
{
}

static void cmd_sleep(void)
{
	k_sleep(K_SECONDS(1));
}

void consumer(void *context)
{
	const struct command *cmd;
	struct in *mem;

	for (;;) {
		printf_P(PSTR("\n# "));

		mem = (struct in *)k_fifo_get(&myfifo, K_FOREVER);

		LOG_HEXDUMP_DBG(mem->buffer, mem->len);

		cmd = find_command(mem->buffer);
		if (cmd != NULL) {
			printf_P(PSTR("\n"));
			cmd_call(cmd);
		}

		free_in(mem);
	}
}

int main(void)
{
	serial_init();
	led_init();

	k_thread_dump_all();

	SET_BIT(UCSR0B, 1 << RXCIE0);

	k_stop();
}