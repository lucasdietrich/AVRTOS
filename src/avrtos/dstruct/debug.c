/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug.h"

#include <avr/pgmspace.h>

void print_slist(struct slist *list, void (*qitem_printer)(struct snode *item))
{
	struct snode *item = list->head;

	serial_transmit('|');
	while (item != NULL) {
		serial_print_p(PSTR(" - "));
		qitem_printer(item);
		item = item->next;
	}
	serial_transmit('\n');
}

//
// TQueue
//
void print_tqueue(struct titem *root, void (*titem_printer)(struct titem *item))
{
	serial_print_p(PSTR("| "));
	struct titem *current = root;
	while (current != NULL) {
		serial_print_p(PSTR("- "));
		titem_printer(current);
		serial_transmit('(');
		serial_u16(current->delay_shift);
		serial_transmit(')');

		current = current->next;
	}
	serial_transmit('\n');
}

void print_dlist(struct dnode *list, void (*ditem_printer)(struct dnode *item))
{
	struct dnode *tmp, *node;

	serial_hex16((uint16_t)list);

	DLIST_FOREACH_SAFE(list, tmp, node)
	{
		serial_print_p(PSTR(" - "));
		ditem_printer(node);
		(void)tmp;
	}

	serial_print_p(PSTR(" - "));
	serial_hex16((uint16_t)list);
	serial_transmit('\n');
}