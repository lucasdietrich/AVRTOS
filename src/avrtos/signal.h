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

#define K_SIGNAL_INIT(sig)                          \
    {                                               \
        .signal = 0u,                               \
        .flags = K_POLL_STATE_NOT_READY,            \
        .waitqueue = DLIST_INIT(sig.waitqueue)      \
    }

#define K_SIGNAL_DEFINE(signal_name) \
    static struct k_signal signal_name = K_SIGNAL_INIT(signal_name)

#define K_SIGNAL_SET_UNREADY(signal) (signal)->flags = K_POLL_STATE_NOT_READY

/*___________________________________________________________________________*/

struct k_signal
{
        uint8_t signal;
        uint8_t flags;
        struct ditem waitqueue;
};

/*___________________________________________________________________________*/

void k_signal_init(struct k_signal *sig);

void k_signal_raise(struct k_signal *sig, uint8_t value);

// poll a single signal, thread "tie" is enough
int8_t k_poll_signal(struct k_signal *sig, k_timeout_t timeout);

// to poll several signal we need as many "tie" (to an event queue) as signals we have
// void k_poll(struct k_signal *signals[], uint8_t size, k_timeout_t timeout);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif