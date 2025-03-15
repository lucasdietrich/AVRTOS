/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/logging.h>
#define LOG_LEVEL LOG_LEVEL_INFO

#define STACK_SIZE 100u

struct task {
    uint16_t id;
    struct snode _handle;
    struct k_thread thread;
    uint8_t stack[STACK_SIZE];

    /* Internal context */
    uint64_t counter;
};

#define BLOCK_SIZE sizeof(struct task)

#if defined(__AVR_ATmega2560__)
#define BLOCKS_COUNT 32u
#else
#define BLOCKS_COUNT 10u
#endif

K_MEM_SLAB_DEFINE(tasks_pool, BLOCK_SIZE, BLOCKS_COUNT);
K_SEM_DEFINE(sem, 0u, 1u);

K_RING_DEFINE(rx_bytes, 2u);

static SLIST_DEFINE(allocated_threads);

static volatile uint32_t counter = 0u;

ISR(USART0_RX_vect)
{
    const char data = USART0_DEVICE->UDRn;

    counter++;

    k_ring_push(&rx_bytes, data);

    if (k_sem_give(&sem)) k_yield_from_isr();
}

static void thread_entry(void *arg)
{
    struct task *task = arg;

    LOG_INF("Task: %p starts counting", task);

    for (;;) {
        task->counter++;
    }
}

static void print_help(void)
{
    LOG_INF("Press:");
    LOG_INF("\tx - to spawn a new thread");
    LOG_INF("\tk - to kill the oldest thread");
    LOG_INF("\tc - to print stack canaries");
    LOG_INF("\tv - to print thread counters");
    LOG_INF("\ty - to yield");
    LOG_INF("\ts - to sleep");
}

static void handle_data(char data)
{
    switch (data) {
    case 'k': {
        /* Kill oldest thread */
        struct snode *node = slist_get(&allocated_threads);

        if (node) {
            struct task *task = CONTAINER_OF(node, struct task, _handle);
            k_thread_abort(&task->thread);
            k_mem_slab_free(&tasks_pool, (void *)task);
            LOG_INF("Killed thread: %p", task);
        } else {
            LOG_ERR("No thread to kill");
        }

        break;
    }
    case 'y':
        k_yield();
        break;
    case 'x': {
        static uint16_t thread_id = 0u;
        /* Spawn a new thread */
        struct task *task;
        if (k_mem_slab_alloc(&tasks_pool, (void **)&task, K_NO_WAIT) == 0) {

            task->id      = thread_id++;
            task->counter = 0u;

            k_thread_create(&task->thread, thread_entry, task->stack, STACK_SIZE,
                            K_PREEMPTIVE, task, 't');
            slist_append(&allocated_threads, &task->_handle);
            k_thread_start(&task->thread);

            LOG_INF("Spawned thread: %p", task);
        } else {
            LOG_ERR("Failed to allocate memory for new thread");
        }
        break;
    }
    case 'c': {
        k_dump_stack_canaries();

        struct snode *node = allocated_threads.head;
        while (node) {
            struct task *task = CONTAINER_OF(node, struct task, _handle);
            k_print_stack_canaries(&task->thread);
            node = node->next;
        }
        break;
    }
    case 'v': {
        struct snode *node = allocated_threads.head;
        while (node) {
            struct task *task = CONTAINER_OF(node, struct task, _handle);
            LOG_INF("TASK id: %u counter: %08lx%08lx", task->id,
                    (uint32_t)(task->counter >> 32u), (uint32_t)(task->counter));
            node = node->next;
        }
        break;
    }
    case 's':
        k_sleep(K_MSEC(200));
        break;
    default:
        print_help();
        break;
    }
}

int main(void)
{
    char data;
    uint32_t counter_copy;
    uint8_t key;

    serial_init();
    ll_usart_enable_rx_isr(USART0_DEVICE);

    print_help();

    for (;;) {
        k_sem_take(&sem, K_FOREVER);

        key          = irq_lock();
        counter_copy = counter;
        irq_unlock(key);

        while (k_ring_pop(&rx_bytes, &data) == 0) {
            LOG_INF("[%lu]\tdata: %c", counter_copy, data);

            handle_data(data);
        }
    }
}