/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Arduino.h>

#include <avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/logging.h>
#include <avrtos/misc/led.h>
#include <avrtos/misc/serial.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "avrtos/sys.h"
#define LOG_LEVEL LOG_LEVEL_WRN

void consumer(void *arg);

static struct k_thread consumer_thread;
static uint8_t consumer_stack[0x100];

struct in {
    struct snode tie;
    char buffer[20];
    uint8_t len;
};

K_MEM_SLAB_DEFINE(myslab, sizeof(struct in), 2u);
K_FIFO_DEFINE(myfifo);

void push(struct in **mem)
{
    struct k_thread *thread = k_fifo_put(&myfifo, *(snode **)mem);
    *mem                    = NULL;
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

__always_inline void input(const char rx)
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
    char name[20u];
    uint8_t name_len;
    void (*func)(void);
};

static void cmd_help(void);
static void cmd_version(void);
static void cmd_sleep(void);
static void cmd_threads(void);
static void cmd_canaries(void);

#define CMD(_name, _func)                                                                \
    {                                                                                    \
        _name, sizeof(_name) - 1u, _func                                                 \
    }

const struct command commands[] PROGMEM = {
    CMD("help", cmd_help),        CMD("version", cmd_version),
    CMD("uptime", k_show_uptime), CMD("ticks", k_show_ticks),
    CMD("sleep", cmd_sleep),      CMD("canaries", cmd_canaries),
    CMD("threads", cmd_threads),  CMD("led", led_toggle),
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
    printf_P(PSTR("version: %02u.%02u.%02u"), AVRTOS_VERSION_MAJOR, AVRTOS_VERSION_MINOR,
             AVRTOS_VERSION_REVISION);
}

static void cmd_sleep(void)
{
    k_sleep(K_SECONDS(1));
}

static void cmd_threads(void)
{
    k_thread_dump(&z_thread_main);
    k_thread_dump(&z_thread_idle);
    k_thread_dump(&consumer_thread);
}

static void cmd_canaries(void)
{
    k_print_stack_canaries(&z_thread_main);
    k_print_stack_canaries(&z_thread_idle);
    k_print_stack_canaries(&consumer_thread);
}

void consumer(void *arg)
{
    ARG_UNUSED(arg);

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

void setup(void)
{
    led_init();
    serial_init_baud(9600u);

    k_mem_slab_init(&myslab, myslab.allocator.buffer, myslab.allocator.block_size,
                    myslab.allocator.count);
    k_thread_create(&consumer_thread, consumer, consumer_stack, sizeof(consumer_stack),
                    K_PREEMPTIVE, NULL, 'A');
    k_thread_start(&consumer_thread);

    cmd_threads();
    cmd_canaries();

    ll_usart_enable_rx_isr(USART0_DEVICE);
}

void loop(void)
{
    // ...
    k_sleep(K_SECONDS(1u));
}