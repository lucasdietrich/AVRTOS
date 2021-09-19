#ifndef _AVRTOS_SIGNAL_H_
#define _AVRTOS_SIGNAL_H_

#include <avr/io.h>
#include <stdbool.h>

#include "multithreading.h"

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

#define K_POLL_STATE_NOT_READY      0x00
#define K_POLL_STATE_SIGNALED       0x01

/*___________________________________________________________________________*/

#define K_SIGNAL_INIT(initial_value)                                                \
    {                                                                               \
        .signal = initial_value, .flags = K_POLL_STATE_NOT_READY, .waitqueue = NULL \
    }

#define K_SIGNAL_DEFINE(signal_name, initial_value) \
    static struct k_signal signal_name = K_SIGNAL_INIT(initial_value)

/*___________________________________________________________________________*/

struct k_signal
{
    uint8_t signal;
    uint8_t flags;
    struct ditem *waitqueue;
};

/*___________________________________________________________________________*/

void k_signal_init(struct k_signal *sig, uint8_t initial_value);

void k_signal_raise(struct k_signal *sig);

// poll a single signal, thread "tie" is enough
uint8_t k_poll_signal(struct k_signal *sig, k_timeout_t timeout);

// as to poll several signal we need as many "tie" to an event queue as signals we have
void k_poll(struct k_signal *signals[], uint8_t size, k_timeout_t timeout);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif