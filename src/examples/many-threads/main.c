/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avr/io.h>
#include <avrtos/kernel.h>
#include <avrtos/drivers/usart.h>

void task(void *);
void stats(void *);
void calc(void *);

K_THREAD_DEFINE(ts, stats, 0x80, K_COOPERATIVE, NULL, '-');
K_THREAD_DEFINE(tc, calc, 0x60, K_PREEMPTIVE, NULL, 'c');

#define AVAILABLE_SRAM (RAMEND - RAMSTART - \
	CONFIG_THREAD_MAIN_STACK_SIZE - \
	_K_THREAD_STACK_SIZE(ts) - _K_THREAD_STACK_SIZE(tc) - 100)

#define THREAD_STACK_SIZE 	0x60
#define THREADS_COUNT 		(AVAILABLE_SRAM / (sizeof(struct k_thread) + THREAD_STACK_SIZE))

static struct k_thread threads[THREADS_COUNT];
static uint8_t stacks[THREADS_COUNT][THREAD_STACK_SIZE];


int main(void)
{
	struct usart_config cfg = USART_CONFIG_DEFAULT_500000();
	usart_init(USART0_DEVICE, &cfg);

	printf_P(PSTR("Many threads sample\n"));

	for (uint8_t i = 0u; i < THREADS_COUNT; i++) {
		k_thread_create(&threads[i], task,
				stacks[i], THREAD_STACK_SIZE,
				K_COOPERATIVE, NULL, 'A' + i);
		k_thread_start(&threads[i]);
	}

	for (;;) {
		
	}
}

void task(void *arg)
{
	ARG_UNUSED(arg);

	for (;;) {
		printf_P(PSTR("%c"), _current->symbol);
		k_sleep(K_MSEC(1000u));
	}
}

void stats(void *arg)
{
	ARG_UNUSED(arg);

	for (;;)
	{
		printf_P(PSTR("\n\n"));
		dump_stack_canaries();
		for (uint8_t i = 0u; i < THREADS_COUNT; i++) {
			print_stack_canaries(&threads[i]);
		}
		k_sleep(K_SECONDS(10));
	}
}

static uint64_t i = 0u;

void calc(void *arg)
{
	ARG_UNUSED(arg);

	for (;;)
	{
		i++;
	}
}