/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file main.c
 * @brief Polling API demonstration example.
 *
 * This example demonstrates the use of the k_poll() function to wait on multiple
 * kernel objects simultaneously. The example sets up various kernel objects
 * (semaphore, mutex, FIFO, message queues) and uses polling to wait for events
 * on these objects.
 *
 * The example uses UART input to trigger events:
 * - 's' or '1': Give semaphore sem1
 * - 'm' or '2': Signal mutex thread to release mutex1
 * - 'f' or '3': Signal FIFO producer to add item to fifo1
 * - 'p' or '4': Signal message queue putter to add message to msgq_get1
 * - 'g' or '5': Signal message queue getter to get message from msgq_put1
 *
 * The main thread polls on all these objects and handles events when they occur.
 */

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/drivers/usart.h>
#include <avrtos/logging.h>
#include <avrtos/poll.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define LOG_LEVEL LOG_LEVEL_DEBUG

/* Objects to poll on */
K_SEM_DEFINE(sem1, 0u, 1u);
K_MUTEX_DEFINE(mutex1);
K_FIFO_DEFINE(fifo1);
#define SZ 8u
#define NB 4u
K_MSGQ_DEFINE(msgq_put1, SZ, NB);
K_MSGQ_DEFINE(msgq_get1, SZ, NB);

/* Semaphores to signal events from UART ISR to worker threads */
K_SEM_DEFINE(sem_mutex, 0u, 1u);
K_SEM_DEFINE(sem_fifo, 0u, 1u);
K_SEM_DEFINE(sem_msgq_put, 0u, 1u);
K_SEM_DEFINE(sem_msgq_get, 0u, 1u);

int main(void)
{
    int ret;

    serial_init();
    ll_usart_enable_rx_isr(USART0_DEVICE);
    k_thread_dump_all();

    k_msleep(100);

    /* Set up poll file descriptors for all objects we want to monitor */
    struct k_pollfd fds[] = {
        {
            .obj.sem = &sem1,
            .type    = K_POLL_TYPE_SEM,
        },
        {
            .obj.mutex = &mutex1,
            .type      = K_POLL_TYPE_MUTEX,
        },
        {
            .obj.fifo = &fifo1,
            .type     = K_POLL_TYPE_FIFO,
        },
        {
            .obj.msgq = &msgq_put1,
            .type     = K_POLL_TYPE_MSGQ_PUT,
        },
        {
            .obj.msgq = &msgq_get1,
            .type     = K_POLL_TYPE_MSGQ_GET,
        },
    };

    while (1) {
        k_timeout_t timeout = K_MSEC(1000);
        ret                 = k_poll(fds, ARRAY_SIZE(fds), timeout);
        if (ret == 0) {
            LOG_INF("poll ok: 0");
            /* Check which objects are ready and handle them */
            if (fds[0].revents & K_POLL_READY) {
                LOG_INF("sem1 ready");
                ret = k_sem_take(&sem1, K_NO_WAIT);
                k_assert(ret == 0);
            }
            if (fds[1].revents & K_POLL_READY) {
                LOG_INF("mutex1 ready");
                ret = k_mutex_lock(&mutex1, K_NO_WAIT);
                LOG_INF("mutex1 locked: %d", ret);
                k_assert(ret == 0);
                k_mutex_unlock(&mutex1);
            }
            if (fds[2].revents & K_POLL_READY) {
                LOG_INF("fifo1 ready");
                struct snode *item = k_fifo_get(&fifo1, K_NO_WAIT);
                k_assert(item != NULL);
                LOG_INF("fifo1 item received: %p", item);
            }
            if (fds[3].revents & K_POLL_READY) {
                LOG_INF("msgq_put1 space available for sending");
                uint8_t msg[SZ] = "MSGQPUT";
                ret             = k_msgq_put(&msgq_put1, msg, K_NO_WAIT);
                k_assert(ret == 0);
                LOG_INF("msgq_put1 message sent");
            }
            if (fds[4].revents & K_POLL_READY) {
                LOG_INF("msgq_get1 message available for receiving");
                uint8_t msg[SZ];
                ret = k_msgq_get(&msgq_get1, msg, K_NO_WAIT);
                k_assert(ret == 0);
                LOG_INF("msgq_get1 message received: %s", msg);
            }
        } else if (ret == -ETIMEDOUT) {
            LOG_INF("timeout: %d", ret);
        } else {
            LOG_INF("error: %d", ret);
        }

        k_msleep(50);
    }
}

/**
 * @brief UART receive interrupt handler.
 *
 * This ISR reads characters from UART and signals the appropriate semaphores
 * to trigger events in the worker threads.
 */
ISR(USART0_RX_vect)
{
    const char chr = USART0_DEVICE->UDRn;
    ll_usart_sync_putc(USART0_DEVICE, chr);

    switch (chr) {
    case 's':
    case '1':
        k_sem_give(&sem1);
        break;
    case 'm':
    case '2':
        k_sem_give(&sem_mutex);
        break;
    case 'f':
    case '3':
        k_sem_give(&sem_fifo);
        break;
    case 'p':
    case '4':
        k_sem_give(&sem_msgq_put);
        break;
    case 'g':
    case '5':
        k_sem_give(&sem_msgq_get);
        break;
    default:
        ll_usart_sync_putc(USART0_DEVICE, '?');
        break;
    }
}

/**
 * @brief Mutex demonstration thread.
 *
 * This thread acquires the mutex, waits for a signal, then releases it.
 */
static void thread_mutex(void *p1);
K_THREAD_DEFINE(tm, thread_mutex, 0x123, K_COOPERATIVE, NULL, 'm');

static void thread_mutex(void *p1)
{
    (void)p1;

    while (1) {
        LOG_INF("acquiring mutex1");
        k_mutex_lock(&mutex1, K_FOREVER);
        LOG_INF("thread1 acquired mutex1");
        k_sem_take(&sem_mutex, K_FOREVER);
        LOG_INF("releasing mutex1");
        k_mutex_unlock(&mutex1);
        k_msleep(100);
    }
}

/**
 * @brief FIFO producer thread.
 *
 * This thread waits for signals and produces items for the FIFO.
 */
static void thread_fifo_producer(void *p1);
K_THREAD_DEFINE(tfp, thread_fifo_producer, 0x123, K_COOPERATIVE, NULL, 'f');

static void thread_fifo_producer(void *p1)
{
    (void)p1;
    static snode_t nodes[4];
    static uint8_t idx = 0;

    while (1) {
        k_sem_take(&sem_fifo, K_FOREVER);
        LOG_INF("fifo producer producing item %u", idx);
        k_fifo_put(&fifo1, &nodes[idx]);
        idx = (idx + 1) % ARRAY_SIZE(nodes);
        k_msleep(100);
    }
}

/**
 * @brief Message queue producer thread.
 *
 * This thread puts messages into the message queue that can be retrieved
 * via polling.
 */
static void thread_msgq_putter(void *p1);
K_THREAD_DEFINE(tmp, thread_msgq_putter, 0x123, K_COOPERATIVE, NULL, 'p');

static void thread_msgq_putter(void *p1)
{
    (void)p1;
    int ret;
    static uint8_t idx = 0;
    uint8_t msg[SZ];

    /* Fill the msgq with some initial messages */
    do {
        snprintf_P((char *)msg, SZ, PSTR("MSGQ%u"), idx);
        LOG_INF("msgq putter sending message: %s", msg);
        ret = k_msgq_put(&msgq_get1, msg, K_NO_WAIT);
        idx++;
    } while (ret != -ENOMEM);

    while (1) {
        k_sem_take(&sem_msgq_put, K_FOREVER);
        snprintf_P((char *)msg, SZ, PSTR("MSGQ%u"), idx);
        LOG_INF("msgq putter sending message: %s", msg);
        k_msgq_put(&msgq_get1, msg, K_FOREVER);
        idx++;
    }
}

/**
 * @brief Message queue consumer thread.
 *
 * This thread gets messages from the message queue that was filled by polling.
 */
static void thread_msgq_getter(void *p1);
K_THREAD_DEFINE(tmg, thread_msgq_getter, 0x123, K_COOPERATIVE, NULL, 'g');

static void thread_msgq_getter(void *p1)
{
    (void)p1;
    int ret;
    uint8_t msg[SZ];

    while (1) {
        k_sem_take(&sem_msgq_get, K_FOREVER);
        ret = k_msgq_get(&msgq_put1, msg, K_FOREVER);
        k_assert(ret == 0);
        LOG_INF("msgq getter received message: %s", msg);
    }
}