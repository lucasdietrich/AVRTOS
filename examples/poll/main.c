/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file main.c
 * @brief Polling API demonstration example with unified producer thread.
 *
 * This example demonstrates the use of the k_poll() function to wait on multiple
 * kernel objects simultaneously. The example sets up various kernel objects
 * (semaphore, mutex, FIFO, message queues) and uses polling to wait for events
 * on these objects.
 *
 * The example uses UART input to trigger events:
 * - 's' or '1': Give semaphore sem1
 * - 'm' or '2': Signal unified producer to handle mutex operation
 * - 'f' or '3': Signal unified producer to add item to fifo1
 * - 'p' or '4': Signal unified producer to add message to msgq_get1
 * - 'g' or '5': Signal unified producer to get message from msgq_put1
 * - 'b' or '6': Signal unified producer to free memory slab block
 *
 * The main thread polls on kernel objects, while a unified producer thread
 * uses k_poll to monitor interrupt-triggered semaphores and handle all
 * producer operations in a single thread.
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
K_MEM_SLAB_DEFINE(mem_slab, SZ, NB);
K_FIFO_DEFINE(mem_slab_allocated_fifo);

/* Semaphores to signal events from UART ISR to worker threads */
K_SEM_DEFINE(sem_mutex, 0u, 1u);
K_SEM_DEFINE(sem_fifo_put, 0u, 1u);
K_SEM_DEFINE(sem_msgq_put, 0u, 1u);
K_SEM_DEFINE(sem_msgq_get, 0u, 1u);
K_SEM_DEFINE(sem_memslab_free, 0u, 1u);

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
        {
            .obj.mem_slab = &mem_slab,
            .type         = K_POLL_TYPE_MEM_SLAB,
        },
    };

    while (1) {
        k_timeout_t timeout = K_MSEC(1000);
        ret                 = k_poll(fds, ARRAY_SIZE(fds), timeout);
        if (ret >= 0) {
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
            if (fds[5].revents & K_POLL_READY) {
                LOG_INF("mem_slab block available for allocation");
                void *mem;
                ret = k_mem_slab_alloc(&mem_slab, &mem, K_NO_WAIT);
                k_assert(ret == 0);
                LOG_INF("mem_slab block allocated: %p", mem);

                /* Store the allocated block in a FIFO for later freeing */
                k_fifo_put(&mem_slab_allocated_fifo, mem);
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
        k_sem_give(&sem_fifo_put);
        break;
    case 'p':
    case '4':
        k_sem_give(&sem_msgq_put);
        break;
    case 'g':
    case '5':
        k_sem_give(&sem_msgq_get);
        break;
    case 'b':
    case '6':
        k_sem_give(&sem_memslab_free);
        break;
    default:
        ll_usart_sync_putc(USART0_DEVICE, '?');
        break;
    }
}

/**
 * @brief Unified producer thread using k_poll.
 *
 * This thread uses k_poll to monitor multiple semaphores triggered by interrupts
 * and handles all producer operations in a single thread.
 */
static void thread_unified_producer(void *p1);
K_THREAD_DEFINE(tup, thread_unified_producer, 0x200, K_COOPERATIVE, NULL, 'u');

static void thread_unified_producer(void *p1)
{
    (void)p1;
    int ret;

    /* Static variables for state management across different operations */
    static snode_t nodes[4];
    static uint8_t fifo_idx = 0;
    static uint8_t msgq_idx = 0;
    uint8_t msg[SZ];
    void *mem;

    /* Initialize msgq_get1 with some initial messages */
    do {
        snprintf_P((char *)msg, SZ, PSTR("MSGQ%u"), msgq_idx);
        ret = k_msgq_put(&msgq_get1, msg, K_NO_WAIT);
        msgq_idx++;
    } while (ret != -ENOMEM);

    /* Pre-acquire the mutex so the main thread can't acquire it initially */
    LOG_INF("Pre-acquiring mutex1 for polling demonstration");
    k_mutex_lock(&mutex1, K_FOREVER);
    LOG_INF("Unified producer has pre-acquired mutex1");

    LOG_INF("Unified producer thread started, polling on interrupt semaphores");

    /* Set up poll descriptors for interrupt-triggered semaphores */
    struct k_pollfd sem_fds[] = {
        K_POLLFD_SEM(&sem_mutex),        K_POLLFD_SEM(&sem_fifo_put),
        K_POLLFD_SEM(&sem_msgq_put),     K_POLLFD_SEM(&sem_msgq_get),
        K_POLLFD_SEM(&sem_memslab_free),
    };

    while (1) {
        /* Poll on all interrupt-triggered semaphores */
        ret = k_poll(sem_fds, ARRAY_SIZE(sem_fds), K_FOREVER);

        if (ret >= 0) {
            /* Handle mutex semaphore - release mutex for main thread to acquire */
            if (sem_fds[0].revents & K_POLL_READY) {
                LOG_INF("sem_mutex ready - releasing mutex for main thread");
                ret = k_sem_take(&sem_mutex, K_NO_WAIT);
                k_assert(ret == 0);

                LOG_INF("releasing mutex1");
                k_mutex_unlock(&mutex1);
                LOG_INF("mutex1 released, main thread can now acquire it via polling");

                /* Let the main thread run a bit to acquire the mutex */
                k_yield();
                LOG_INF("re-acquiring mutex1 for next cycle");
                k_mutex_lock(&mutex1, K_FOREVER);
            }

            /* Handle FIFO producer semaphore */
            if (sem_fds[1].revents & K_POLL_READY) {
                LOG_INF("sem_fifo_put ready - producing fifo item");
                ret = k_sem_take(&sem_fifo_put, K_NO_WAIT);
                k_assert(ret == 0);

                LOG_INF("fifo producer producing item %u", fifo_idx);
                k_fifo_put(&fifo1, &nodes[fifo_idx]);
                fifo_idx = (fifo_idx + 1) % ARRAY_SIZE(nodes);
            }

            /* Handle message queue producer semaphore */
            if (sem_fds[2].revents & K_POLL_READY) {
                LOG_INF("sem_msgq_put ready - producing msgq message");
                ret = k_sem_take(&sem_msgq_put, K_NO_WAIT);
                k_assert(ret == 0);

                snprintf_P((char *)msg, SZ, PSTR("MSGQ%u"), msgq_idx);
                LOG_INF("msgq putter sending message: %s", msg);
                k_msgq_put(&msgq_get1, msg, K_FOREVER);
                msgq_idx++;
            }

            /* Handle message queue consumer semaphore */
            if (sem_fds[3].revents & K_POLL_READY) {
                LOG_INF("sem_msgq_get ready - consuming msgq message");
                ret = k_sem_take(&sem_msgq_get, K_NO_WAIT);
                k_assert(ret == 0);

                ret = k_msgq_get(&msgq_put1, msg, K_FOREVER);
                k_assert(ret == 0);
                LOG_INF("msgq getter received message: %s", msg);
            }

            /* Handle memory slab free semaphore */
            if (sem_fds[4].revents & K_POLL_READY) {
                LOG_INF("sem_memslab_free ready - freeing memory slab block");
                ret = k_sem_take(&sem_memslab_free, K_NO_WAIT);
                k_assert(ret == 0);

                mem = k_fifo_get(&mem_slab_allocated_fifo, K_FOREVER);
                if (mem != NULL) {
                    k_mem_slab_free(&mem_slab, mem);
                    LOG_INF("mem_slab block freed: %p", mem);
                }
            }
        } else {
            LOG_ERR("k_poll error in unified producer: %d", ret);
        }

        k_dump_stack_canaries();
    }
}
