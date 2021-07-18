#include "debug.h"

void print_queue(struct qitem *root, void (*qitem_printer)(struct qitem *item))
{
    usart_transmit('|');
    while (root != NULL)
    {
        usart_print(" - ");
        qitem_printer(root);
        root = root->next;
    }
    usart_transmit('\n');
}

void print_dlist(struct ditem *ref, void (*ditem_printer)(struct ditem *item))
{
    ditem_printer(ref);
    uint16_t counter = 0;

    struct ditem *current = ref->next;
    while (current != ref)
    {
        usart_print(" > ");
        ditem_printer(current);
        counter++;
        current = current->next;
    }
    // usart_print(" > ");
    // ditem_printer(current);
    usart_print(" [");
    usart_u16(counter);
    usart_print("]\n");
}