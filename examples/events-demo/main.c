/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>
#include <avrtos/misc/serial.h>
#include <avrtos/drivers/timer.h>

#include <util/delay.h>

#define K_MODULE K_MODULE_EVENT

K_EVENT_Q_DEFINE(my_evq);

#define PERIOD_MS 1lu

ISR(TIMER3_COMPA_vect)
{
    k_event2_q_advance(&my_evq, PERIOD_MS);
}

struct Event {
    struct k_event2 ev;
    k_ticks_t timeout; // user
};

struct Event events[] = {
    {
        .ev = K_EVENT2_INIT(),
        .timeout = 500,
    },
    {
        .ev = K_EVENT2_INIT(),
        .timeout = 1000,
    },
    {
        .ev = K_EVENT2_INIT(),
        .timeout = 5000,
    },
    {
        .ev = K_EVENT2_INIT(),
        .timeout = 10000,
    },
};

int main(void)
{
    const struct timer_config cfg = {
        .mode      = TIMER_MODE_CTC,
        .prescaler = TIMER_PRESCALER_1024,
        .counter   = TIMER_CALC_COUNTER_VALUE(PERIOD_MS * 1000lu, 1024u),
        .timsk     = BIT(OCIEnA),
    };

    int8_t ret = timer16_init(TIMER3_DEVICE, &cfg);
    if (ret)
        printf_P(PSTR("timer16_init failed: %d\n"), ret);

    k_event2_schedule(&my_evq, &events[0].ev, events[0].timeout);
    k_event2_schedule(&my_evq, &events[1].ev, events[1].timeout);
    k_event2_schedule(NULL, &events[2].ev, events[2].timeout);
    k_event2_schedule(NULL, &events[3].ev, events[3].timeout);
    
    struct k_pollfd pfds[2u];
    struct k_event2 *ev;

    pfds[0].type = K_POLL_TYPE_EVENT2;
    pfds[0].obj.evq = &my_evq;
    pfds[1].type = K_POLL_TYPE_EVENT2;
    pfds[1].obj.evq = K_KERNEL_EVQ;

    for (;;) {
        ret = k_poll(pfds, ARRAY_SIZE(pfds), K_FOREVER);
        if (ret >= 0) {
            for (uint_fast8_t i = 0; i < ARRAY_SIZE(pfds); i++) {
                if (pfds[i].revents & K_POLL_READY) {
                    ret = k_event2_pend(pfds[i].obj.evq, K_NO_WAIT, &ev);
                    printf_P(PSTR("pfds[%u] is ready - k_event2_pend ret: %d ev: %p\n"), i, ret, (void *)ev);

                    struct Event *myev = CONTAINER_OF(ev, struct Event, ev);
                    k_event2_schedule(pfds[i].obj.evq, ev, myev->timeout);
                }
            }
        } else {
            printf_P(PSTR("k_poll error: %d\n"), ret);
        }
    }
}