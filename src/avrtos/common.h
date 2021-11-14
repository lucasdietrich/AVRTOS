#ifndef _AVRTOS_COMMON_H
#define _AVRTOS_COMMON_H

/*___________________________________________________________________________*/

/* non standard */
#define ENOERROR        0
#define EERROR          1
#define ETIMEOUT        2
#define ECANCEL         3

/* standard */
#define EINTR 4
#define EIO 5
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define EBUSY 16
#define EINVAL 22
#define ENOMSG 35

#define EWOULDBLOCK EAGAIN

/*___________________________________________________________________________*/

/* arch */
#define NOINLINE            __attribute__((noinline))
#define NORETURN            __attribute__((__noreturn__))
#define CODE_UNREACHABLE    __builtin_unreachable();

/*___________________________________________________________________________*/

#define K_TIMEOUT_EQ(t1, t2)    (t1.value == t2.value)

#define K_SECONDS(delay_s)      ((k_timeout_t){.value = 1000u*delay_s})
#define K_MSEC(delay_ms)        ((k_timeout_t){.value = delay_ms})
#define K_NO_WAIT               ((k_timeout_t){.value = (k_delta_ms_t) 0})
#define K_FOREVER               ((k_timeout_t){.value = (k_delta_ms_t) -1})
#define K_UNTIL_WAKEUP          K_FOREVER

#define HTONL(n) ((((((uint32_t)(n) & 0xFF)) << 24) |      \
               ((((uint32_t)(n) & 0xFF00)) << 8) |         \
               ((((uint32_t)(n) & 0xFF0000)) >> 8) |       \
               ((((uint32_t)(n) & 0xFF000000)) >> 24)))

#define HTONS(n) (((((uint16_t)(n) & 0xFF)) << 8) |       \
               ((((uint16_t)(n) & 0xFF00)) >> 8))

#define K_SWAP_ENDIANNESS(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))

#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))

#define SET_BIT(x, b)  ((x) |= b)
#define CLR_BIT(x, b)  ((x) &= (~(b)))
#define TEST_BIT(x, b) ((bool) ((x) & b))

#define ARG_UNUSED(arg) ((void) arg)

#define IN_RANGE(x, a, b) ((x >= a) && (x <= b))

/*___________________________________________________________________________*/

#define PROGMEM_STRING(name, string)            \
    static const char name[] PROGMEM = string

#define PRINT_PROGMEM_STRING(name, string)      \
    static const char name[] PROGMEM = string;  \
    usart_print_p(name)

/*___________________________________________________________________________*/

#define K_FLAG_SCHED_LOCKED_SHIFT   2
#define K_FLAG_SCHED_LOCKED         (1 << K_FLAG_SCHED_LOCKED_SHIFT)

#define K_FLAG_PREEMPT_SHIFT        3

#define K_FLAG_COOP                 (1 << K_FLAG_PREEMPT_SHIFT)
#define K_FLAG_PREEMPT              (0 << K_FLAG_PREEMPT_SHIFT)

#define K_FLAG_STOPPED              (0b00 << 0)
#define K_FLAG_READY                (0b01 << 0)
#define K_FLAG_PENDING              (0b10 << 0)

#define K_FLAG_PRIO_SHIFT           4
#define K_FLAG_PRIO                 (1 << K_FLAG_PRIO_SHIFT)

#define K_PRIO_HIGH                 0b1
#define K_PRIO_LOW                  0b0

#define K_FLAG_PRIO_HIGH            (K_PRIO_HIGH << K_FLAG_PRIO_SHIFT)
#define K_FLAG_PRIO_LOW             (K_PRIO_LOW << K_FLAG_PRIO_SHIFT)

// (K_FLAG_READY | K_FLAG_COOP | ((p & 0b11) << 3))
#define K_PRIO_PREEMPT(p)           (K_FLAG_PREEMPT | ((p & 0b1) << K_FLAG_PRIO_SHIFT))

// (K_FLAG_READY | K_FLAG_PREEMPT | ((p & 0b11) << 3))
#define K_PRIO_COOP(p)              (K_FLAG_COOP | ((p & 0b1) << K_FLAG_PRIO_SHIFT))

#define K_COOPERATIVE               K_PRIO_COOP(K_PRIO_HIGH)
#define K_PREEMPTIVE                K_PRIO_PREEMPT(K_PRIO_HIGH)

#define K_PRIO_DEFAULT              K_PREEMPTIVE
#define K_STOPPED                   K_FLAG_STOPPED

#define K_FLAG_PEND_CANCELED_SHIFT  5
#define K_FLAG_PEND_CANCELED        (1 << K_FLAG_PEND_CANCELED_SHIFT)

#define K_FLAG_TIMER_EXPIRED_SHIFT  6
#define K_FLAG_TIMER_EXPIRED        (1 << K_FLAG_TIMER_EXPIRED_SHIFT)

/*___________________________________________________________________________*/

#endif