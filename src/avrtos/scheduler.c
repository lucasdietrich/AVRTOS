#include "scheduler.h"

#include "multithreading.h"


// A should be processed in 10 ms
// B should be processed in 10 + 30 = 40ms
// C should be processed in 40 + 0 = 40ms
// D should be processed in 40 + 10 = 50ms
// 0 |-- A(10) -- B(30) -- C(0) -- D(10)

// adding E which should be processed in 25ms

// we need to insert i beetween A and B in order to have :
// 0 |-- A(10) -- E(15) -- B(15) -- C(0) -- D(10)


/*___________________________________________________________________________*/

// is volatile required ?
static struct k_scheduled_item_t* scheduled_items_root = NULL;

struct k_scheduled_item_t* _k_schedule_get_root(void)
{
    return scheduled_items_root;
}

// scheduled_item_t must have abs_delay set and thread (next = NULL)
void _k_schedule_submit(struct k_scheduled_item_t *new_item)
{
    if (new_item == NULL)
        return;
    new_item->next = NULL;  // safe

    cli();
    struct k_scheduled_item_t ** prev_next_p = &scheduled_items_root;
    while (*prev_next_p != NULL)    // if next of previous item is set
    {
        struct k_scheduled_item_t * p_current = *prev_next_p; // next of previous become current

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

void _k_schedule_time_passed(k_delta_ms_t delta)
{
    if (!delta)
        return;

    cli();
    struct k_scheduled_item_t ** prev_next_p = &scheduled_items_root;
    while (*prev_next_p != NULL) // if next of previous item is set
    {
        struct k_scheduled_item_t * p_current = *prev_next_p; // next of previous become current
        if (p_current->delay_shift < delta)
        {
            delta -= p_current->delay_shift;
            p_current->delay_shift = 0;
            prev_next_p = &(p_current->next);
        }
        else
        {
            p_current->delay_shift -= delta;
            break;
        }
    }
    sei();
}

struct k_scheduled_item_t * _k_schedule_pop_first_expired()
{
    cli();
    struct k_scheduled_item_t * item = NULL;
    if ((scheduled_items_root != NULL) && (scheduled_items_root->delay_shift == 0)) // if next to expire has expired
    {
        item = scheduled_items_root;    // prepare to return it
        scheduled_items_root = scheduled_items_root->next;  // set next
    }
    sei();
    return item;
}

/*___________________________________________________________________________*/