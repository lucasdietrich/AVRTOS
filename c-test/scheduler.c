#include <stdio.h>
#include <stdint.h>

// UBUNTU
// gcc scheduler.c -o scheduler
// ./scheduler

/*___________________________________________________________________________*/

typedef uint32_t scheduler_delay_t;

struct time_ms_t
{
    scheduler_delay_t delay;
};

struct scheduled_item_t
{
    union
    {
        scheduler_delay_t delay_shift;
        scheduler_delay_t abs_delay;
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
    // CLI
    if (scheduled_items_root == NULL)
    {
        scheduled_items_root = new_item;
        new_item->next = NULL;
    }
    

    // check first item
    if (scheduled_items_root->delay_shift > new_item->delay_shift)
    {
        struct scheduled_item_t* temp = scheduled_items_root;
        scheduled_items_root = new_item;
        temp->delay_shift -= new_item->delay_shift;
        new_item->next = temp;
        return;
    }
    else
    {
        new_item->delay_shift -= scheduled_items_root->delay_shift;
    }

    struct scheduled_item_t** p_item = &scheduled_items_root;
    while ((*p_item)->next != NULL)
    {
        // if difference is strict, new item will be inserted before oldest item if they have same expiration delay (MORE CALCULATION + 1 loop)
        // if difference is large, new item will be inserted after oldest item if they have same expiration delay (OLDEST item executed first)
        if ((*p_item)->next->delay_shift <= new_item->delay_shift)
        {
            new_item->delay_shift -= (*p_item)->next->delay_shift;
            p_item = &(*p_item)->next;
        }
        else
        {
            break;
        }
    }

    // we need to insert the item between (*p_item) and (*p_item)->next

    // if not end of chained list
    if ((*p_item)->next != NULL)
    {
        new_item->next = (*p_item)->next;
        (*p_item)->next->delay_shift -= new_item->delay_shift;     // adjust delay

        // if delay_shift = 0 for few items, TODO order depending of priority
        // if ((*p_item)->next->delay_shift == 0) { }
    }
    (*p_item)->next = new_item;

    // SEI
}

void k_sleep(struct time_ms_t wait)
{

}

uint8_t k_mutex_lock(uint8_t *mutex, struct time_ms_t wait)
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
    uint8_t count = 0;

    printf("0 |");

    while (item != NULL)
    {
        print_scheduled_item(item);
        item = item->next;
        count++;
    }
    printf("\ncount = %d\n", count);
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
}