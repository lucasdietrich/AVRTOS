/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "debug.h"

void print_queue(struct qitem *root, void (*qitem_printer)(struct qitem *item))
{
        serial_transmit('|');
        while (root != NULL) {
                serial_print_p(PSTR(" - "));
                qitem_printer(root);
                root = root->next;
        }
        serial_transmit('\n');
}

void print_oqueue(struct oqref *oref, void (*qitem_printer)(struct qitem *item))
{
        struct qitem *item = oref->head;
        serial_print_p(PSTR("[H]"));
        while (item != NULL) {
                serial_print_p(PSTR(" > "));
                qitem_printer(item);
                item = item->next;
        }
        serial_printl_p(PSTR(" > [T]"));
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

void print_dlist(struct ditem *list, void (*ditem_printer)(struct ditem *item))
{
	struct ditem *tmp, *node;

	serial_hex16((uint16_t) list);

	DLIST_FOREACH_SAFE(list, tmp, node) {
		serial_print_p(PSTR(" - "));
		ditem_printer(node);
		(void) tmp;
	}

	serial_print_p(PSTR(" - "));
	serial_hex16((uint16_t) list);
	serial_transmit('\n');
}