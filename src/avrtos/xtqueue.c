#include "xtqueue.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// A should be processed in 10 ms
// B should be processed in 10 + 30 = 40ms
// C should be processed in 40 + 0 = 40ms
// D should be processed in 40 + 10 = 50ms
// 0 |-- A(10) -- B(30) -- C(0) -- D(10)

// adding E which should be processed in 25ms

// we need to insert i beetween A and B in order to have :
// 0 |-- A(10) -- E(15) -- B(15) -- C(0) -- D(10)


/*___________________________________________________________________________*/


// scheduled_item_t must have abs_delay set and thread (next = NULL)
void k_xtqueue_schedule(struct k_xtqueue_item_t **root, struct k_xtqueue_item_t *new_item)
{
    if (new_item == NULL)
        return;
    new_item->next = NULL;  // safe

    cli();
    struct k_xtqueue_item_t ** prev_next_p = root;
    while (*prev_next_p != NULL)    // if next of previous item is set
    {
        struct k_xtqueue_item_t * p_current = *prev_next_p; // next of previous become current

        // if current element expires before or at the same time that new_timer, we go to next item
        if (p_current->delay_shift <= new_item->delay_shift)
        {
            new_item->delay_shift -= p_current->delay_shift;
            prev_next_p = &(p_current->next);
        }
        else    // if current element expire after, we need to insert the new_item before current and linked *prev_next_p
        {
            new_item->next = p_current;
            p_current->delay_shift -= new_item->delay_shift;     // adjust delay
            break;

            // if delay_shift = 0 for few items, TODO order depending of priority
            // if ((*p_item)->next->delay_shift == 0) { }
        }
    }
    *prev_next_p = new_item;
    sei();
}

void k_xtqueue_shift(struct k_xtqueue_item_t **root, k_delta_ms_t time_passed)
{
    if (!time_passed)
        return;

    cli();
    struct k_xtqueue_item_t ** prev_next_p = root;
    while (*prev_next_p != NULL) // if next of previous item is set
    {
        struct k_xtqueue_item_t * p_current = *prev_next_p; // next of previous become current
        if (p_current->delay_shift < time_passed)
        {
            time_passed -= p_current->delay_shift;
            p_current->delay_shift = 0;
            prev_next_p = &(p_current->next);
        }
        else
        {
            p_current->delay_shift -= time_passed;
            break;
        }
    }
    sei();
}

struct k_xtqueue_item_t * k_xtqueue_pop(struct k_xtqueue_item_t **root)
{
    cli();
    struct k_xtqueue_item_t * item = NULL;
    if ((root != NULL) && ((*root)->delay_shift == 0)) // if next to expire has expired
    {
        item = *root;    // prepare to return it
        *root = (*root)->next;  // set next
    }
    sei();
    return item;
}

/*___________________________________________________________________________*/