#include <stdio.h>
#include <stdint.h>

/*___________________________________________________________________________*/

// UBUNTU
// gcc scheduler.c -o scheduler
// ./scheduler

//
// OUTPUT
//

// INIT
// 0 |-- A(10) -- B(20) -- C(50)
// ADDING -- D(25)
// 0 |-- A(10) -- D(15) -- B(5) -- C(50)
// ADDING -- E(10)
// 0 |-- A(10) -- E(0) -- D(15) -- B(5) -- C(50)
// ADDING -- F(25)
// 0 |-- A(10) -- E(0) -- D(15) -- F(0) -- B(5) -- C(50)
// ADDING -- G(3)
// 0 |-- G(3) -- A(7) -- E(0) -- D(15) -- F(0) -- B(5) -- C(50)
// _schedule_time_passed (1)
// 0 |-- G(2) -- A(7) -- E(0) -- D(15) -- F(0) -- B(5) -- C(50)
// _schedule_pop_first_expired = 0
// 0 |-- G(2) -- A(7) -- E(0) -- D(15) -- F(0) -- B(5) -- C(50)
// _schedule_time_passed (3)
// 0 |-- G(0) -- A(6) -- E(0) -- D(15) -- F(0) -- B(5) -- C(50)
// _schedule_pop_first_expired = 1
// 0 |-- A(6) -- E(0) -- D(15) -- F(0) -- B(5) -- C(50)
// _schedule_time_passed (7)
// 0 |-- A(0) -- E(0) -- D(14) -- F(0) -- B(5) -- C(50)
// _schedule_pop_first_expired = 1
// 0 |-- E(0) -- D(14) -- F(0) -- B(5) -- C(50)
// _schedule_pop_first_expired = 1
// 0 |-- D(14) -- F(0) -- B(5) -- C(50)
// _schedule_pop_first_expired = 0
// 0 |-- D(14) -- F(0) -- B(5) -- C(50)
// _schedule_time_passed (100)
// 0 |-- D(0) -- F(0) -- B(0) -- C(0)
// _schedule_pop_first_expired = 1
// 0 |-- F(0) -- B(0) -- C(0)
// _schedule_pop_first_expired = 1
// 0 |-- B(0) -- C(0)
// _schedule_pop_first_expired = 1
// 0 |-- C(0)
// _schedule_pop_first_expired = 1
// 0 |
// _schedule_pop_first_expired = 0
// 0 |

/*___________________________________________________________________________*/

typedef uint16_t delta_ms_t;

void sei() { }
void cli() { }

struct time_ms_t
{
    delta_ms_t delay;
};

struct scheduled_item_t
{
    union
    {
        delta_ms_t delay_shift;
        delta_ms_t abs_delay;
    };    
    char thread_letter;
    struct scheduled_item_t* next;
};

#define K_MSEC(delay) ((struct time_ms_t){delay})
#define K_NO_WAIT(delay) ((struct time_ms_t){0})
#define K_FOREVER(delay) ((struct time_ms_t){-1})

/*___________________________________________________________________________*/

static struct scheduled_item_t* scheduled_items_root = NULL;

// scheduled_item_t must have abs_delay set and thread (next = NULL)
void _schedule_event(struct scheduled_item_t *new_item)
{
    if (new_item == NULL)
        return;
    new_item->next = NULL;  // safe

    cli();
    struct scheduled_item_t ** prev_next_p = &scheduled_items_root;
    while (*prev_next_p != NULL)    // if next of previous item is set
    {
        struct scheduled_item_t * p_current = *prev_next_p; // next of previous become current

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

void _schedule_time_passed(delta_ms_t delta)
{
    if (!delta)
        return;

    cli();
    struct scheduled_item_t ** prev_next_p = &scheduled_items_root;
    while (*prev_next_p != NULL) // if next of previous item is set
    {
        struct scheduled_item_t * p_current = *prev_next_p; // next of previous become current
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

struct scheduled_item_t * _schedule_pop_first_expired()
{
    cli();
    struct scheduled_item_t * item = NULL;
    if ((scheduled_items_root != NULL) && (scheduled_items_root->delay_shift == 0)) // if next to expire has expired
    {
        item = scheduled_items_root;    // prepare to return it
        scheduled_items_root = scheduled_items_root->next;  // set next
    }
    sei();
    return item;
}

void k_sleep(struct time_ms_t timeout)
{

}

uint8_t k_mutex_lock(uint8_t *mutex, struct time_ms_t timeout)
{

}

void k_mutex_release(uint8_t *mutex)
{

}

/*___________________________________________________________________________*/

void print_scheduled_item(struct scheduled_item_t* const item)
{
    printf("-- %c(%d) ", item->thread_letter, item->delay_shift);
}

void print_scheduled_items_list()
{
    struct scheduled_item_t* item = scheduled_items_root;

    printf("0 |");

    while (item != NULL)
    {
        print_scheduled_item(item);
        item = item->next;
    }
    printf("\n");
}

/*___________________________________________________________________________*/

int main(void)
{
    struct scheduled_item_t items[] = {
        { 10, 'A'},
        { 20, 'B'},
        { 50, 'C'},
    };

    // init 
    scheduled_items_root = &items[0];
    items[0].next = &items[1];
    items[1].next = &items[2];

    printf("INIT\n");
    print_scheduled_items_list();

    struct scheduled_item_t new_itemD = { 25, 'D'};
    printf("ADDING ");
    print_scheduled_item(&new_itemD);
    printf("\n");
    _schedule_event(&new_itemD);
    print_scheduled_items_list();

    struct scheduled_item_t new_itemE = { 10, 'E'};
    printf("ADDING ");
    print_scheduled_item(&new_itemE);
    printf("\n");
    _schedule_event(&new_itemE);
    print_scheduled_items_list();

    struct scheduled_item_t new_itemF = { 25, 'F'};
    printf("ADDING ");
    print_scheduled_item(&new_itemF);
    printf("\n");
    _schedule_event(&new_itemF);
    print_scheduled_items_list();

    struct scheduled_item_t new_itemG = { 3, 'G'};
    printf("ADDING ");
    print_scheduled_item(&new_itemG);
    printf("\n");
    _schedule_event(&new_itemG);
    print_scheduled_items_list();

    printf("_schedule_time_passed (1)");
    _schedule_time_passed(1);
    printf("\n");
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();

    printf("_schedule_time_passed (3)");
    _schedule_time_passed(3);
    printf("\n");
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();

    printf("_schedule_time_passed (7)");
    _schedule_time_passed(7);
    printf("\n");
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();

    printf("_schedule_time_passed (100)");
    _schedule_time_passed(100);
    printf("\n");
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();

    printf("_schedule_pop_first_expired = %d\n", _schedule_pop_first_expired() != NULL);
    print_scheduled_items_list();
}