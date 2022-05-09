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
#define EBADF 9
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define EBUSY 16
#define EINVAL 22
#define ENFILE 23
#define ENOMSG 35


#define EWOULDBLOCK EAGAIN

/*___________________________________________________________________________*/

/* arch */
#define NOINLINE            __attribute__((noinline))
#define NORETURN            __attribute__((__noreturn__))
#define CODE_UNREACHABLE    __builtin_unreachable();

/*___________________________________________________________________________*/

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

#define BIT(b) (1 << (b))
#define SET_BIT(x, b)  ((x) |= b)
#define CLR_BIT(x, b)  ((x) &= (~(b)))
#define TEST_BIT(x, b) ((bool) ((x) & b))

#define ARG_UNUSED(arg) ((void) arg)

#define IN_RANGE(x, a, b) ((x >= a) && (x <= b))

/*___________________________________________________________________________*/

#define K_FLAG_SCHED_LOCKED_SHIFT   2
#define K_FLAG_SCHED_LOCKED         (1 << K_FLAG_SCHED_LOCKED_SHIFT)

#define K_FLAG_PREEMPT_SHIFT        3

#define K_FLAG_COOP                 (1 << K_FLAG_PREEMPT_SHIFT)
#define K_FLAG_PREEMPT              (0 << K_FLAG_PREEMPT_SHIFT)

#define K_FLAG_STOPPED              (0b00 << 0)
#define K_FLAG_READY                (0b01 << 0)
#define K_FLAG_PENDING              (0b10 << 0)

#define K_FLAG_INTPREEMPT_SHIFT     4
#define K_FLAG_INTPREEMPT           (1 << K_FLAG_INTPREEMPT_SHIFT)

#define K_COOPERATIVE               K_FLAG_COOP
#define K_PREEMPTIVE                K_FLAG_PREEMPT

#define K_PRIO_DEFAULT              K_PREEMPTIVE

#define K_FLAG_PEND_CANCELED_SHIFT  5
#define K_FLAG_PEND_CANCELED        (1 << K_FLAG_PEND_CANCELED_SHIFT)

#define K_FLAG_TIMER_EXPIRED_SHIFT  6
#define K_FLAG_TIMER_EXPIRED        (1 << K_FLAG_TIMER_EXPIRED_SHIFT)

/*___________________________________________________________________________*/

#endif