#ifndef _AVRTOS_COMMON_H
#define _AVRTOS_COMMON_H

/*___________________________________________________________________________*/

#define K_PRIO_MAX                  0b00
#define K_PRIO_HIGH                 0b01
#define K_PRIO_LOW                  0b10
#define K_PRIO_MIN                  0b11

#define K_FLAG_COOP                 (1 << 2)
#define K_FLAG_PREEMPT              (0 << 2)

#define K_FLAG_STOPPED              (0b00 << 0)
#define K_FLAG_RUNNING              (0b01 << 0)
#define K_FLAG_READY                (0b10 << 0)
#define K_FLAG_WAITING              (0b11 << 0)

#define K_FLAG_PRIO_SHIFT           3

#define K_FLAG_PRIO_MAX             (K_PRIO_MAX << K_FLAG_PRIO_SHIFT)
#define K_FLAG_PRIO_HIGH            (K_PRIO_HIGH << K_FLAG_PRIO_SHIFT)
#define K_FLAG_PRIO_LOW             (K_PRIO_LOW << K_FLAG_PRIO_SHIFT)
#define K_FLAG_PRIO_MIN             (K_PRIO_MIN << K_FLAG_PRIO_SHIFT)

// (K_FLAG_READY | K_FLAG_COOP | ((p & 0b11) << 3))
#define K_PRIO_PREEMPT(p)           (K_FLAG_PREEMPT | ((p & 0b11) << K_FLAG_PRIO_SHIFT))

// (K_FLAG_READY | K_FLAG_PREEMPT | ((p & 0b11) << 3))
#define K_PRIO_COOP(p)              (K_FLAG_COOP | ((p & 0b11) << K_FLAG_PRIO_SHIFT))

#define K_PRIO_DEFAULT              K_PRIO_PREEMPT(K_PRIO_HIGH)
#define K_STOPPED                   K_FLAG_STOPPED

#define K_FLAG_IMMEDIATE_SHIFT       5
#define K_FLAG_IMMEDIATE             (1 << K_FLAG_IMMEDIATE_SHIFT)

// #define K_FLAG_IMMINENT_SHIFT       6
// #define K_FLAG_IMMINENT             1 << K_FLAG_IMMINENT_SHIFT

#define SYSCLOCK_PRESCALER_1        ((1 << CS00) | (0 << CS01) |  (0 << CS02))
#define SYSCLOCK_PRESCALER_8        ((0 << CS00) | (1 << CS01) |  (0 << CS02))
#define SYSCLOCK_PRESCALER_64       ((1 << CS00) | (1 << CS01) |  (0 << CS02))
#define SYSCLOCK_PRESCALER_256      ((0 << CS00) | (0 << CS01) |  (1 << CS02))
#define SYSCLOCK_PRESCALER_1024     ((1 << CS00) | (0 << CS01) |  (1 << CS02))




/*___________________________________________________________________________*/

#endif