/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos.h>

static struct k_thread thread_led;
static uint8_t thread_led_stack[128u];
static void thread_led_task(void *args);

void setup(void)
{
    /* LED initialisation */
    pinMode(LED_BUILTIN, OUTPUT);

    /* Serial initialisation */
    Serial.begin(9600u);

    /* Create thread_led handling LED, then start it */
    k_thread_create(&thread_led, thread_led_task, thread_led_stack,
                    sizeof(thread_led_stack), K_PREEMPTIVE, NULL, 'L');
    k_thread_start(&thread_led);

    /* Make main thread_led preemptive, so that the LED thread_led can run
     * even if the main thread_led is running */
    k_thread_set_priority(k_thread_get_main(), K_PREEMPTIVE);

    Serial.println("Application started");
}

void loop(void)
{
    // ...
}

static void thread_led_task(void *args)
{
    (void)args;

    while (1) {
        digitalWrite(LED_BUILTIN, HIGH);
        k_sleep(K_MSEC(200u));
        digitalWrite(LED_BUILTIN, LOW);
        k_sleep(K_MSEC(200u));
    }
}