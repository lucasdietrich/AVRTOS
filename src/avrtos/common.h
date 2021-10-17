#ifndef _AVRTOS_COMMON_H
#define _AVRTOS_COMMON_H

/*___________________________________________________________________________*/

#define K_PRIO_MAX                  0b00
#define K_PRIO_HIGH                 0b01
#define K_PRIO_LOW                  0b10
#define K_PRIO_MIN                  0b11

#define K_FLAG_SCHED_LOCKED_SHIFT   2
#define K_FLAG_SCHED_LOCKED         (1 << K_FLAG_SCHED_LOCKED_SHIFT)

#define K_FLAG_PREEMPT_SHIFT        3

#define K_FLAG_COOP                 (1 << K_FLAG_PREEMPT_SHIFT)
#define K_FLAG_PREEMPT              (0 << K_FLAG_PREEMPT_SHIFT)

#define K_FLAG_STOPPED              (0b00 << 0)
#define K_FLAG_READY                (0b01 << 0)
#define K_FLAG_WAITING              (0b10 << 0)

#define K_FLAG_PRIO_SHIFT           4

#define K_FLAG_PRIO_MAX             (K_PRIO_MAX << K_FLAG_PRIO_SHIFT)
#define K_FLAG_PRIO_HIGH            (K_PRIO_HIGH << K_FLAG_PRIO_SHIFT)
#define K_FLAG_PRIO_LOW             (K_PRIO_LOW << K_FLAG_PRIO_SHIFT)
#define K_FLAG_PRIO_MIN             (K_PRIO_MIN << K_FLAG_PRIO_SHIFT)

// (K_FLAG_READY | K_FLAG_COOP | ((p & 0b11) << 3))
#define K_PRIO_PREEMPT(p)           (K_FLAG_PREEMPT | ((p & 0b11) << K_FLAG_PRIO_SHIFT))

// (K_FLAG_READY | K_FLAG_PREEMPT | ((p & 0b11) << 3))
#define K_PRIO_COOP(p)              (K_FLAG_COOP | ((p & 0b11) << K_FLAG_PRIO_SHIFT))

#define K_COOPERATIVE               K_PRIO_COOP(K_PRIO_HIGH)
#define K_PREEMPTIVE                K_PRIO_PREEMPT(K_PRIO_HIGH)

#define K_PRIO_DEFAULT              K_PREEMPTIVE
#define K_STOPPED                   K_FLAG_STOPPED

#define K_FLAG_TIMER_EXPIRED_SHIFT   6
#define K_FLAG_TIMER_EXPIRED         (1 << K_FLAG_TIMER_EXPIRED_SHIFT)

/*___________________________________________________________________________*/

#endif