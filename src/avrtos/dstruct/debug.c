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

void print_dlist(struct ditem *dlist, void (*ditem_printer)(struct ditem *item))
{
        uint16_t counter = 0;

        if (dlist != NULL) {
                usart_print_p(PSTR("[H]"));
                struct ditem *current = dlist->head;
                while (current != dlist) {
                        usart_print_p(PSTR(" > "));
                        ditem_printer(current);
                        counter++;
                        current = current->next;
                }
                usart_print_p(PSTR(" > [T]"));

                current = dlist->tail;
                while (current != dlist) {
                        usart_print_p(PSTR(" > "));
                        ditem_printer(current);
                        current = current->prev;
                }
                usart_print_p(PSTR(" > [H]"));
        }

        usart_print_p(PSTR(" ("));
        usart_u16(counter);
        usart_print_p(PSTR(")\n"));
}

void print_ref_dlist(struct ditem *ref,
        void (*ditem_printer)(struct ditem *item))
{
        uint16_t counter = 0;

        if (ref != NULL) {
                counter++;
                ditem_printer(ref);

                struct ditem *current = ref->next; // prev
                while (current != ref) {
                        usart_print_p(PSTR(" > "));
                        ditem_printer(current);
                        counter++;
                        current = current->next; // prev
                }
        }

        usart_print_p(PSTR(" BACKWARD "));

        if (ref != NULL) {
                ditem_printer(ref);

                struct ditem *current = ref->prev; // prev
                while (current != ref) {
                        usart_print_p(PSTR(" > "));
                        ditem_printer(current);
                        current = current->prev; // prev
                }
        }

        usart_print_p(PSTR(" ("));
        usart_u16(counter);
        usart_print_p(PSTR(")\n"));
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