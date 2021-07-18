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
    uint16_t counter = 0;

    if (ref != NULL)
    {
        counter++;
        ditem_printer(ref);

        struct ditem *current = ref->next;
        while (current != ref)
        {
            usart_print(" > ");
            ditem_printer(current);
            counter++;
            current = current->next;
        }
    }

    usart_print(" [");
    usart_u16(counter);
    usart_print("]\n");
}

//
// TQueue
//
void print_tqueue(struct titem *root, void (*titem_printer)(struct titem *item))
{
    usart_print("| ");
    struct titem *current = root;
    while (current != NULL)
    {
        usart_print("- ");
        titem_printer(current);
        current = current->next;
    }
    usart_transmit('\n');
}