/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug.h"

void print_queue(struct qitem *root, void (*qitem_printer)(struct qitem *item))
{
        usart_transmit('|');
        while (root != NULL) {
                usart_print_p(PSTR(" - "));
                qitem_printer(root);
                root = root->next;
        }
        usart_transmit('\n');
}

void print_oqueue(struct oqref *oref, void (*qitem_printer)(struct qitem *item))
{
        struct qitem *item = oref->head;
        usart_print_p(PSTR("[H]"));
        while (item != NULL) {
                usart_print_p(PSTR(" > "));
                qitem_printer(item);
                item = item->next;
        }
        usart_printl_p(PSTR(" > [T]"));
}

//
// TQueue
//
void print_tqueue(struct titem *root, void (*titem_printer)(struct titem *item))
{
        usart_print_p(PSTR("| "));
        struct titem *current = root;
        while (current != NULL) {
                usart_print_p(PSTR("- "));
                titem_printer(current);
                usart_transmit('(');
                usart_u16(current->delay_shift);
                usart_transmit(')');

                current = current->next;
        }
        usart_transmit('\n');
}

void print_dlist(struct ditem *list, void (*ditem_printer)(struct ditem *item))
{
	struct ditem *tmp, *node;

	usart_hex16((uint16_t) list);

	DLIST_FOREACH_SAFE(list, tmp, node) {
		usart_print_p(PSTR(" - "));
		ditem_printer(node);
		(void) tmp;
	}

	usart_print_p(PSTR(" - "));
	usart_hex16((uint16_t) list);
	usart_transmit('\n');
}