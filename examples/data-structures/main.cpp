/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*___________________________________________________________________________*/

#include <avrtos/defines.h>
#include <avrtos/dstruct/debug.h>
#include <avrtos/dstruct/dlist.h>
#include <avrtos/dstruct/slist.h>
#include <avrtos/dstruct/tqueue.h>
#include <avrtos/misc/serial.h>

#include <avr/io.h>

/*___________________________________________________________________________*/

#define FLAG_SLIST  1
#define FLAG_DLIST  4
#define FLAG_TQUEUE 8

#define TESTS FLAG_SLIST | FLAG_DLIST | FLAG_TQUEUE

void test_slist(void);
void test_dlis(void);
void test_tqueue(void);

/*___________________________________________________________________________*/

int main(void)
{
	serial_init();

#if TESTS & FLAG_SLIST
	serial_printl_p(PSTR("queue"));
	test_slist();
#endif

#if TESTS & FLAG_DLIST
	serial_printl_p(PSTR("queue/dequeue dlist"));
	test_dlis();
#endif

	// todo raw api

	// todo cdlist

#if TESTS & FLAG_TQUEUE
	serial_printl_p(PSTR("tqueue"));
	test_tqueue();
#endif
}

/*___________________________________________________________________________*/

//
// SLIST
//

struct item {
	const char name;
	struct snode i;
};

#define ITEM(name)                                                                       \
	{                                                                                \
		name,                                                                    \
		{                                                                        \
			0                                                                \
		}                                                                        \
	}
#define ITEM_OF(snode) ((struct item *)CONTAINER_OF(snode, struct item, i))
#define NAME_OF(snode) ITEM_OF(snode)->name

struct item items[] = {ITEM('A'), ITEM('B'), ITEM('C'), ITEM('D'), ITEM('E')};

SLIST_DEFINE(myqueue);

inline void queue(struct item *item)
{
	return slist_append(&myqueue, &item->i);
}
inline struct item *dequeue(void)
{
	struct snode *item = slist_get(&myqueue);
	if (item == NULL) {
		return NULL;
	} else {
		return ITEM_OF(item);
	}
}
void print_slist_item(struct snode *item)
{
	serial_transmit(NAME_OF(item));
}
inline void print_slist(void)
{
	print_slist(&myqueue, print_slist_item);
}

void test_slist(void)
{
	queue(&items[0]);
	queue(&items[1]);
	queue(&items[2]);

	struct item *dequeued = NULL;

	print_slist();
	dequeued = dequeue();
	print_slist();
	serial_print_p(PSTR("queue "));
	print_slist_item(&dequeued->i);
	serial_transmit('\n');
	queue(dequeued);
	print_slist();
	dequeued = dequeue();
	print_slist();
	dequeued = dequeue();
	print_slist();
	serial_print_p(PSTR("queue "));
	print_slist_item(&dequeued->i);
	serial_transmit('\n');
	queue(dequeued);
	print_slist();
	dequeued = dequeue();
	print_slist();
}

/*___________________________________________________________________________*/

//
// DLIST
//

struct item2 {
	const char name;
	struct dnode i;
};

#define DITEM(name)                                                                      \
	{                                                                                \
		name,                                                                    \
		{                                                                        \
			0, 0                                                             \
		}                                                                        \
	}
#define DITEM_OF(item) ((struct item2 *)CONTAINER_OF(item, struct item2, i))
#define DNAME_OF(item) DITEM_OF(item)->name
#define DEFINE_DLIST_ITEM(dlist_item)                                                    \
	struct item2 dlist_item = {'*', {&dlist_item.i, &dlist_item.i}}

struct item2 ditems[] = {DITEM('A'), DITEM('B'), DITEM('C'), DITEM('D'), DITEM('E')};

DEFINE_DLIST(dlist);

void print_dlist_item(struct dnode *item)
{
	serial_transmit(DNAME_OF(item));
}
inline void print_dlist(void)
{
	print_dlist(&dlist, print_dlist_item);
}

static void test_dlist_dequeue(struct dnode *dlist)
{
	struct dnode *out = dlist_get(dlist);
	if (DITEM_VALID(dlist, out)) {
		print_dlist_item(out);
		serial_transmit('\n');
		print_dlist(dlist, print_dlist_item);
	} else {
		serial_print_p(PSTR("dlist empty="));
		serial_u8(dlist_is_empty(dlist));
	}
}

void test_dlis(void)
{
	print_dlist(&dlist, print_dlist_item);
	dlist_append(&dlist, &ditems[0].i);
	print_dlist(&dlist, print_dlist_item);
	dlist_append(&dlist, &ditems[1].i);
	print_dlist(&dlist, print_dlist_item);
	dlist_append(&dlist, &ditems[2].i);
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
	{'A', {100}}, {'B', {25}}, {'C', {35}}, {'D', {35}}, {'E', {25}}};

void print_titem(struct titem *item)
{
	struct item3 *i = CONTAINER_OF(item, struct item3, tie);
	serial_transmit(i->chr);
	serial_transmit('[');
	serial_u16(i->tie.delay_shift);
	serial_transmit(']');
}

void print_tqueue(struct titem *root)
{
	print_tqueue(root, print_titem);
}

void test_tqueue(void)
{
	struct titem *root = NULL;

	for (uint8_t i = 0; i < 5; i++) {
		z_tqueue_schedule(&root, &titems[4 - i].tie);
	}

	print_tqueue(root);

	tqueue_shift(&root, 20);

	print_tqueue(root);

	tqueue_shift(&root, 20);

	print_tqueue(root);

	struct titem *pop = tqueue_pop(&root);
	print_titem(pop);
	serial_transmit('\n');
	pop = tqueue_pop(&root);
	print_titem(pop);
	serial_transmit('\n');

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