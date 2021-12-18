/*___________________________________________________________________________*/

#include <avr/io.h>

#include <avrtos/misc/uart.h>

#include <avrtos/dstruct/dlist.h>
#include <avrtos/dstruct/queue.h>
#include <avrtos/dstruct/tqueue.h>
#include <avrtos/dstruct/debug.h>

#include <avrtos/defines.h>

/*___________________________________________________________________________*/

#define FLAG_QUEUE        1
#define FLAG_OQUEUE       2
#define FLAG_DLIST        4
#define FLAG_TQUEUE       8

#define TESTS             FLAG_OQUEUE

void test_queue(void);
void test_oqueue(void);
void test_queue_dlist(void);
void test_tqueue(void);

/*___________________________________________________________________________*/

int main(void)
{
  usart_init();

#if TESTS & FLAG_QUEUE
  usart_printl_p(PSTR("queue"));
  test_queue();
#endif

#if TESTS & FLAG_OQUEUE
  usart_printl_p(PSTR("oqueue"));
  test_oqueue();
#endif

#if TESTS & FLAG_DLIST
  usart_printl_p(PSTR("queue/dequeue dlist"));
  test_queue_dlist();
#endif

  // todo raw api

  // todo cdlist

#if TESTS & FLAG_TQUEUE
  usart_printl_p(PSTR("tqueue"));
  test_tqueue();
#endif
}

/*___________________________________________________________________________*/

//
// QUEUE
//

struct item
{
  const char name;
  struct qitem i;
};

#define ITEM(name) {name, {0}}
#define ITEM_OF(qitem) ((struct item*) CONTAINER_OF(qitem, struct item, i))
#define NAME_OF(qitem) ITEM_OF(qitem)->name


struct item items[] = {
  ITEM('A'), ITEM('B'), ITEM('C'), ITEM('D'), ITEM('E')
};

DEFINE_QUEUE(myqueue);

inline void queue(struct item *item) { return queue(&myqueue, &item->i); }
inline struct item *dequeue(void)
{
  struct qitem *item = dequeue(&myqueue);
  if (item == NULL)
  {
    return NULL;
  }
  else
  {
    return ITEM_OF(item);
  }
}
void print_queue_item(struct qitem *item) { usart_transmit(NAME_OF(item)); }
inline void print_queue(void) { print_queue(myqueue, print_queue_item); }

void test_queue(void)
{
  queue(&items[0]);
  queue(&items[1]);
  queue(&items[2]);

  struct item * dequeued = NULL;

  print_queue();
  dequeued = dequeue();
  print_queue();
  usart_print_p(PSTR("queue "));
  print_queue_item(&dequeued->i);
  usart_transmit('\n');
  queue(dequeued);
  print_queue();
  dequeued = dequeue();
  print_queue();
  dequeued = dequeue();
  print_queue();
  usart_print_p(PSTR("queue "));
  print_queue_item(&dequeued->i);
  usart_transmit('\n');
  queue(dequeued);
  print_queue();
  dequeued = dequeue();
  print_queue();
}

/*___________________________________________________________________________*/

DEFINE_OQREF(oref);

inline void print_oqueue(void) { print_oqueue(&oref, print_queue_item); }

void print_odequeue(void)
{
  struct qitem* dequeued = odequeue(&oref);
  if (dequeued != NULL) {
    usart_print_p(PSTR("dequeued : "));
    print_queue_item(dequeued);
    usart_transmit('\n');
  } else {
    usart_printl_p(PSTR("dequeued NULL"));
  }
  print_oqueue();
}

//
// OQUEUE
//
void test_oqueue(void)
{
  print_oqueue();
  oqueue(&oref, &items[0].i);
  print_oqueue();
  oqueue(&oref, &items[1].i);
  oqueue(&oref, &items[2].i);
  print_oqueue();

  print_odequeue();
  print_odequeue();
  print_odequeue();
  print_odequeue();
}

/*___________________________________________________________________________*/

//
// DLIST
//

struct item2
{
  const char name;
  struct ditem i;
};

#define DITEM(name) {name, {0, 0}}
#define DITEM_OF(item) ((struct item2*) CONTAINER_OF(item, struct item2, i))
#define DNAME_OF(item) DITEM_OF(item)->name
#define DEFINE_DLIST_ITEM(dlist_item) struct item2 dlist_item = {'*', {&dlist_item.i, &dlist_item.i} }

struct item2 ditems[] = {
  DITEM('A'), DITEM('B'), DITEM('C'), DITEM('D'), DITEM('E')
};

DEFINE_DLIST(dlist);

void print_dlist_item(struct ditem *item) { usart_transmit(DNAME_OF(item)); }
inline void print_dlist(void) { print_dlist(&dlist, print_dlist_item); }

static void test_dlist_dequeue(struct ditem *dlist)
{
  struct ditem *out = dlist_dequeue(dlist);
  if (DITEM_VALID(dlist, out)) {
    print_dlist_item(out);
    usart_transmit('\n');
    print_dlist(dlist, print_dlist_item);
  } else { usart_print_p(PSTR("dlist empty=")); usart_u8(dlist_is_empty(dlist)); }
}


void test_queue_dlist(void)
{
  print_dlist(&dlist, print_dlist_item);
  dlist_queue(&dlist, &ditems[0].i);
  print_dlist(&dlist, print_dlist_item);
  dlist_queue(&dlist, &ditems[1].i);
  print_dlist(&dlist, print_dlist_item);
  dlist_queue(&dlist, &ditems[2].i);
  print_dlist(&dlist, print_dlist_item);

  test_dlist_dequeue(&dlist);
  test_dlist_dequeue(&dlist);
  test_dlist_dequeue(&dlist);
  test_dlist_dequeue(&dlist);
}

/*___________________________________________________________________________*/

//
// TQUEUE
//

struct item3 {
    const char chr;
    struct titem tie;
};

struct item3 titems[] = {
    {'A', {100}},
    {'B', {25}},
    {'C', {35}},
    {'D', {35}},
    {'E', {25}}
};

void print_titem(struct titem *item)
{
    struct item3 *i = CONTAINER_OF(item, struct item3, tie);
    usart_transmit(i->chr);
    usart_transmit('[');
    usart_u16(i->tie.delay_shift);
    usart_transmit(']');
}

void print_tqueue(struct titem *root)
{
  print_tqueue(root, print_titem);
}

void test_tqueue(void)
{
    struct titem *root = NULL;

    for (uint8_t i = 0; i < 5; i++)
    {
        _tqueue_schedule(&root, &titems[4 - i].tie);
    }   

    print_tqueue(root);

    tqueue_shift(&root, 20);

    print_tqueue(root);

    tqueue_shift(&root, 20);

    print_tqueue(root);

    struct titem *pop = tqueue_pop(&root);
    print_titem(pop);
    usart_transmit('\n');
    pop = tqueue_pop(&root);
    print_titem(pop);
    usart_transmit('\n');

    tqueue_shift(&root, 20);

    print_tqueue(root);

    tqueue_remove(&root, &titems[2].tie);

    print_tqueue(root);

    tqueue_remove(&root, &titems[2].tie);

    print_tqueue(root);

    tqueue_remove(&root, &titems[0].tie);

    print_tqueue(root);

    tqueue_remove(&root, &titems[3].tie);

    print_tqueue(root);
}

/*___________________________________________________________________________*/