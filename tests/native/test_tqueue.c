/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Native (host) unit tests for src/avrtos/dstruct/tqueue.c
 *
 * Test cases are described as a small sequence of operations ("ops") replayed
 * against a fresh queue of named items. Add a new case by appending a row to
 * `cases[]` below.
 *
 * Build/run: `make -C tests/native run`
 */

#include <stdio.h>

#include "dstruct/tqueue.h"

static int g_failures = 0;

#define CHECK(cond)                                                                      \
    do {                                                                                 \
        if (!(cond)) {                                                                   \
            fprintf(stderr, "  [%s] op#%d FAILED: %s (%s:%d)\n", case_name, op_idx,      \
                    #cond, __FILE__, __LINE__);                                          \
            g_failures++;                                                                \
        }                                                                                \
    } while (0)

/* named items usable in test cases; NONE stands for "no item"/NULL */
enum item_id {
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    MAX_ITEMS,
    NONE = -1
};

enum op_kind {
    OP_END,
    OP_SCHEDULE,
    OP_SHIFT,
    OP_POP,
    OP_POP_RESCHEDULE,
    OP_RESCHEDULE,
    OP_REMOVE,
    OP_ORDER,
};

#define MAX_ORDER (2 * MAX_ITEMS + 1)

struct op {
    enum op_kind kind;
    int item;             /* item index (enum item_id), or NONE */
    k_delta_t arg;        /* timeout / time_passed, depending on kind */
    int8_t expect_ret;    /* OP_RESCHEDULE: expected return code */
    int order[MAX_ORDER]; /* OP_ORDER: item,delay, item,delay, ..., -1 */
};

#define END               {.kind = OP_END}
#define SCHEDULE(i, t)    {.kind = OP_SCHEDULE, .item = (i), .arg = (t)}
#define SHIFT(t)          {.kind = OP_SHIFT, .arg = (t)}
#define POP(i)            {.kind = OP_POP, .item = (i)}
#define POP_RESCHED(i, t) {.kind = OP_POP_RESCHEDULE, .item = (i), .arg = (t)}
#define RESCHED(i, t, ret)                                                               \
    {.kind = OP_RESCHEDULE, .item = (i), .arg = (t), .expect_ret = (ret)}
#define REMOVE(i)  {.kind = OP_REMOVE, .item = (i)}
#define ORDER(...) {.kind = OP_ORDER, .order = {__VA_ARGS__, -1}}
#define EMPTY()    {.kind = OP_ORDER, .order = {-1}}

#define MAX_OPS 8

struct test_case {
    const char *name;
    struct op ops[MAX_OPS];
};

static const struct test_case cases[] = {
    {"schedule: single item", {SCHEDULE(A, 10), ORDER(A, 10), END}},
    {"schedule: increasing timeouts append in order",
     {SCHEDULE(A, 10), SCHEDULE(B, 40), SCHEDULE(C, 40), SCHEDULE(D, 50),
      ORDER(A, 10, B, 30, C, 0, D, 10), END}},
    {"schedule: insert in the middle",
     {SCHEDULE(A, 10), SCHEDULE(B, 40), SCHEDULE(C, 40), SCHEDULE(D, 50), SCHEDULE(E, 25),
      ORDER(A, 10, E, 15, B, 15, C, 0, D, 10), END}},
    {"schedule: insert before head",
     {SCHEDULE(B, 20), SCHEDULE(A, 5), ORDER(A, 5, B, 15), END}},
    {"schedule: equal timeout keeps insertion order",
     {SCHEDULE(A, 5), SCHEDULE(B, 5), ORDER(A, 5, B, 0), END}},
    {"schedule: three items at the exact same time",
     {SCHEDULE(A, 5), SCHEDULE(B, 5), SCHEDULE(C, 5), ORDER(A, 5, B, 0, C, 0), END}},

    {"shift: partially consumes head", {SCHEDULE(A, 10), SHIFT(4), ORDER(A, 6), END}},
    {"shift: fully consumes head, tail keeps waiting",
     {SCHEDULE(A, 5), SCHEDULE(B, 15), SHIFT(5), ORDER(A, 0, B, 10), END}},
    {"shift: consumes multiple expired items",
     {SCHEDULE(A, 5), SCHEDULE(B, 15), SHIFT(20), ORDER(A, 0, B, 0), END}},
    {"shift: zero time passed is a no-op",
     {SCHEDULE(A, 10), SHIFT(0), ORDER(A, 10), END}},
    {"shift: no-op on an empty queue", {SHIFT(100), EMPTY(), END}},

    {"pop: NULL when nothing expired", {SCHEDULE(A, 10), POP(NONE), ORDER(A, 10), END}},
    {"pop: NULL on an empty queue", {POP(NONE), EMPTY(), END}},
    {"pop: unlinks expired head",
     {SCHEDULE(A, 5), SCHEDULE(B, 15), SHIFT(5), POP(A), ORDER(B, 10), END}},
    {"pop_reschedule: NULL on an empty queue", {POP_RESCHED(NONE, 10), EMPTY(), END}},
    {"pop_reschedule: requeues the popped item",
     {SCHEDULE(A, 5), SCHEDULE(B, 15), SHIFT(5), POP_RESCHED(A, 20), ORDER(B, 10, A, 10),
      END}},

    {"reschedule: moves an item earlier than the head",
     {SCHEDULE(A, 10), SCHEDULE(B, 40), SCHEDULE(C, 50), RESCHED(B, 5, 0),
      ORDER(B, 5, A, 5, C, 40), END}},
    {"reschedule: moves an item later",
     {SCHEDULE(A, 10), SCHEDULE(B, 20), SCHEDULE(C, 30), RESCHED(A, 25, 0),
      ORDER(B, 20, A, 5, C, 5), END}},
    {"reschedule: item not found returns -ENOENT and leaves the queue untouched",
     {SCHEDULE(A, 10), SCHEDULE(B, 20), RESCHED(C, 5, -ENOENT), ORDER(A, 10, B, 10),
      END}},
    {"reschedule: item rescheduled to its own slot is a no-op",
     {SCHEDULE(A, 10), SCHEDULE(B, 20), RESCHED(A, 10, 0), ORDER(A, 10, B, 10), END}},

    {"remove: middle item, successor inherits remaining delay",
     {SCHEDULE(A, 10), SCHEDULE(B, 30), SCHEDULE(C, 35), REMOVE(B), ORDER(A, 10, C, 25),
      END}},
    {"remove: head item",
     {SCHEDULE(A, 10), SCHEDULE(B, 20), REMOVE(A), ORDER(B, 20), END}},
    {"remove: tail item",
     {SCHEDULE(A, 10), SCHEDULE(B, 20), REMOVE(B), ORDER(A, 10), END}},
    {"remove: only item empties the queue", {SCHEDULE(A, 10), REMOVE(A), EMPTY(), END}},
    {"remove: item not in the queue is a no-op",
     {SCHEDULE(A, 10), SCHEDULE(B, 20), REMOVE(C), ORDER(A, 10, B, 10), END}},
};

static void run_case(const struct test_case *tc)
{
    const char *case_name = tc->name;
    int op_idx            = 0;

    struct titem items[MAX_ITEMS] = {0};
    DEFINE_TQUEUE(root);

    for (const struct op *op = tc->ops; op->kind != OP_END; op++, op_idx++) {
        switch (op->kind) {
        case OP_SCHEDULE:
            tqueue_schedule(&root, &items[op->item], op->arg);
            break;
        case OP_SHIFT:
            tqueue_shift(&root, op->arg);
            break;
        case OP_POP:
            CHECK(tqueue_pop(&root) == (op->item == NONE ? NULL : &items[op->item]));
            break;
        case OP_POP_RESCHEDULE:
            CHECK(tqueue_pop_reschedule(&root, op->arg) ==
                  (op->item == NONE ? NULL : &items[op->item]));
            break;
        case OP_RESCHEDULE:
            CHECK(tqueue_reschedule(&root, op->item == NONE ? NULL : &items[op->item],
                                    op->arg) == op->expect_ret);
            break;
        case OP_REMOVE:
            tqueue_remove(&root, &items[op->item]);
            break;
        case OP_ORDER: {
            struct titem *node = root;
            for (int k = 0; op->order[k] != -1; k += 2) {
                CHECK(node == &items[op->order[k]]);
                CHECK(node != NULL && node->delay_shift == (k_delta_t)op->order[k + 1]);
                node = node ? node->next : NULL;
            }
            CHECK(node == NULL);
            break;
        }
        default:
            break;
        }
    }
}

static void test_invalid_args(void)
{
    const char *case_name = "invalid-args";
    int op_idx            = 0;

    DEFINE_TQUEUE(root);
    struct titem a = {0};

    CHECK(tqueue_pop(NULL) == NULL);

    CHECK(tqueue_reschedule(NULL, &a, 5) == -EINVAL);
    CHECK(tqueue_reschedule(&root, NULL, 5) == -EINVAL);

    CHECK(tqueue_remove(NULL, &a) == -EINVAL);
    CHECK(tqueue_remove(&root, NULL) == -EINVAL);
}

int main(void)
{
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        run_case(&cases[i]);
    }
    test_invalid_args();

    if (g_failures == 0) {
        printf("All %zu tqueue test cases passed\n", sizeof(cases) / sizeof(cases[0]));
        return 0;
    }

    fprintf(stderr, "%d check(s) failed\n", g_failures);
    return 1;
}
