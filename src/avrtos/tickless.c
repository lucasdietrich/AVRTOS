#include "tickless.h"
#include <stdio.h>
#include <avr/pgmspace.h>
#include "avrtos/assert.h"
#include "avrtos/kernel.h"
#include "avrtos/sys.h"
#include "drivers/timer.h"
#include "gpio.h"
#include "serial.h"
#include "timer_defs.h"
#include "kernel.h"

#define LOG_LEVEL LOG_LEVEL_DBG
#define K_MODULE K_MODULE_TICKLESS_CLOCK

#include "systime.h"

// (.venv) lucas@zgw:~/AVRTOS$ python3 scripts/timer_calc.py 
// timer0 :  8 bits, prescalers = 1, 8, 64, 256, 1024
// prescaler = 1  freq = 62500.0 Hz (0.0625 us)  period = 16 us
// prescaler = 8  freq = 7812.5 Hz (0.5 us)  period = 128 us
// prescaler = 64  freq = 976.5625 Hz (4.0 us)  period = 1024 us
// prescaler = 256  freq = 244.140625 Hz (16.0 us)  period = 4096 us
// prescaler = 1024  freq = 61.03515625 Hz (64.0 us)  period = 16384 us
// timer1 : 16 bits, prescalers = 1, 8, 64, 256, 1024
// prescaler = 1  freq = 244.140625 Hz (0.0625 us)  period = 4096 us
// prescaler = 8  freq = 30.517578125 Hz (0.5 us)  period = 32768 us
// prescaler = 64  freq = 3.814697265625 Hz (4.0 us)  period = 262144 us
// prescaler = 256  freq = 0.95367431640625 Hz (16.0 us)  period = 1048576 us
// prescaler = 1024  freq = 0.2384185791015625 Hz (64.0 us)  period = 4194304 us
// timer2 :  8 bits, prescalers = 1, 8, 32, 64, 128, 256, 1024
// prescaler = 1  freq = 62500.0 Hz (0.0625 us)  period = 16 us
// prescaler = 8  freq = 7812.5 Hz (0.5 us)  period = 128 us
// prescaler = 32  freq = 1953.125 Hz (2.0 us)  period = 512 us
// prescaler = 64  freq = 976.5625 Hz (4.0 us)  period = 1024 us
// prescaler = 128  freq = 488.28125 Hz (8.0 us)  period = 2048 us
// prescaler = 256  freq = 244.140625 Hz (16.0 us)  period = 4096 us
// prescaler = 1024  freq = 61.03515625 Hz (64.0 us)  period = 16384 us
// Timer 1 : prescaler = 256 , TCNT = 0 : freq = 0.954 Hz -> 1048576 us

#if CONFIG_KERNEL_TICKLESS

#if 0
#warning "Using prescaler 256 for tickless mode, this will result in a 16us tick"
// prescaler = 256  freq = 0.95367431640625 Hz (16.0 us)  period = 1048576 us
#define TICKLESS_PRESCALER TIMER_PRESCALER_256
#define TICKLESS_PRESCALER_VALUE 256u
#define TICKLESS_US_PER_TICK 16u
#define TICKLESS_US_PER_PERIOD 1048576u
#else
#warning "Using prescaler 64 for tickless mode, this will result in a 4us tick"
// prescaler = 64  freq = 3.814697265625 Hz (4.0 us)  period = 262144 us
#define TICKLESS_PRESCALER TIMER_PRESCALER_64
#define TICKLESS_PRESCALER_VALUE 64u
#define TICKLESS_US_PER_TICK 4u
#define TICKLESS_US_PER_PERIOD 262144u
#endif

// // 4 us = 1 << 2
// #define TICKLESS_QUANTUM_US_ORDER 2u
// #define TICKLESS_QUANTUM_US (1u << TICKLESS_QUANTUM_US_ORDER)
// // 262144 us = 1 << 18
// #define TICKLESS_PERIOD_US_ORDER 18lu
// #define TICKLESS_PERIOD_US (1lu << TICKLESS_PERIOD_US_ORDER)
// #define TICKLESS_PERIOD_US_MASK (TICKLESS_PERIOD_US - 1lu)

static uint32_t z_tickless_counter = 0u;

enum tickless_state {
    // Compare Match Register is not set, no scheduling pointer is set
    TICKLESS_IDLE = 0u,
    // Compare Match Register is set
    TICKLESS_SCHEDULED,
    // Compare Match Register is not set, but the timer is waiting
    // for few overflows to reach the next scheduling point frame
    // in order to schedule the next tickless event which is very far 
    // in the future.
    // This typically happens when scheduling a ponter further than
    // the timer overflow period.
    TICKLESS_FAR_SCHEDULING,
};

static enum tickless_state z_tickless_state = TICKLESS_IDLE;
static uint16_t z_tickless_unwrap_counter = 0u;

ISR(TIMER1_COMPA_vect)
{
#if CONFIG_KERNEL_TICKLESS_DEBUG
    gpiol_pin_write_state(GPIOF, 5u, 0u);
    serial_transmit('k');
#endif

    // Disable the interrupt as the event is serviced
    ll_timer16_disable_interrupt(TIMER1_INDEX, OCIEnA);
    z_tickless_state = TICKLESS_IDLE;
}

// ISR(TIMER1_COMPB_vect)
// {
//     gpiol_pin_toggle(GPIOF, 5u);
// }

// ISR(TIMER1_COMPC_vect)
// {
//     gpiol_pin_toggle(GPIOF, 6u);
// }

ISR(TIMER1_OVF_vect)
{
#if CONFIG_KERNEL_TICKLESS_DEBUG
    gpiol_pin_toggle(GPIOF, 7u);
    serial_transmit('o');
#endif

    z_tickless_counter += 1u;

    // If we are in the far scheduling state, we need to decrement the
    // unwrap counter, which is used to track how many overflows we need
    // to wait before we can schedule the next tickless event.
    // - If the unwrap counter is 0, we can schedule the next tickless event.
    // - If the unwrap counter is greater than 0, we just decrement it and return.
    if (z_tickless_state == TICKLESS_FAR_SCHEDULING) {

        if (z_tickless_unwrap_counter > 0u) {
            z_tickless_unwrap_counter -= 1u;
            return;
        }

        // We can now schedule the next tickless event
        ll_timer16_clear_irq_flag(TIMER1_INDEX, OCIEnA);
        ll_timer16_enable_interrupt(TIMER1_INDEX, OCIEnA);
        z_tickless_state = TICKLESS_SCHEDULED;
    }
}

void z_tickless_init(void)
{
    const struct timer_config config = {
        .mode      = TIMER_MODE_NORMAL, 
        .counter   = 0u,
        .prescaler = TICKLESS_PRESCALER,
        .timsk     = BIT(TOIEn), // BIT(OCIEnA)  | BIT(OCIEnB) | BIT(OCIEnC)
    };

    gpiol_pin_init(GPIOF, 4u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 5u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 6u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 7u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);

    // struct timer_channel_compare_config comp_conf[3u] = {
    //     {
    //         .mode  = TIMER_CHANNEL_COMP_MODE_NORMAL,
    //         .value = TIMER_CALC_COUNTER_VALUE(6000, TICKLESS_PRESCALER_VALUE),
    //     },
    //     {
    //         .mode  = TIMER_CHANNEL_COMP_MODE_NORMAL,
    //         .value = TIMER_CALC_COUNTER_VALUE(4000, TICKLESS_PRESCALER_VALUE),
    //     },
    //     {
    //         .mode  = TIMER_CHANNEL_COMP_MODE_NORMAL,
    //         .value = TIMER_CALC_COUNTER_VALUE(2000, TICKLESS_PRESCALER_VALUE),
    //     }
    // };

    // ll_timer16_channel_configure(TIMER1_DEVICE, TIMER_CHANNEL_A, &comp_conf[0]);
    // ll_timer16_channel_configure(TIMER1_DEVICE, TIMER_CHANNEL_B, &comp_conf[1]);
    // ll_timer16_channel_configure(TIMER1_DEVICE, TIMER_CHANNEL_C, &comp_conf[2]);
    
    ll_timer16_channel_set_mode(TIMER1_DEVICE, TIMER_CHANNEL_A, TIMER_CHANNEL_COMP_MODE_NORMAL);
    ll_timer16_channel_set_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A, 0u);
    ll_timer16_init(TIMER1_DEVICE, timer_get_index(TIMER1_DEVICE), &config);
}

__noinline int8_t z_tickless_sched_next_ms(uint32_t ms)
{
    
    if (z_tickless_state != TICKLESS_IDLE) {
        // If the tickless state is not idle, we cannot schedule a new event
        return -EBUSY;
    }
    
    __ASSERT_NOINTERRUPT();
    
    gpiol_pin_write_state(GPIOF, 4u, 1u);
    gpiol_pin_write_state(GPIOF, 5u, 1u);

    // 1ms with 64us tickless quantum becomes is 15.625 ticks which becomes 16 ticks
    uint32_t ticks = (ms * 1000u) / TICKLESS_US_PER_TICK + 1u;

    uint16_t new_counter = ll_timer16_get_tcnt(TIMER1_DEVICE) + ticks;
    
    /* Make sure to test the case when the new counter equals to 0,
    * which means that the TIMER1_COMPA_vect irq will be enabled and triggered 
    * during the execution of the last TIMER1_OVF_vect irq handler.
    * It could be possible to delay the triggering of the TIMER1_COMPA_vect
    * until the next TIMER1_OVF_vect irq handler if its execution is too long.
    */
    ll_timer16_clear_irq_flag(TIMER1_INDEX, OCIEnA);
    ll_timer16_channel_set_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A, new_counter);

    // Check whether it is a far scheduling or not
    ticks >>= 16u;

    if (ticks) {
        // If the ticks are more than 65536, we cannot schedule it in a single
        // timer compare match register, so we will schedule it in the next
        // overflow event.
        z_tickless_unwrap_counter = ticks;
        z_tickless_state = TICKLESS_FAR_SCHEDULING;
    } else {
        ll_timer16_enable_interrupt(TIMER1_INDEX, OCIEnA);
        z_tickless_state = TICKLESS_SCHEDULED;
    }

    gpiol_pin_write_state(GPIOF, 4u, 0u);

    return 0;
}

void z_tickless_time_get(struct z_tickless_timespec *tls)
{
    __ASSERT_NOTNULL(tls);

    uint8_t key = irq_lock();

    tls->hardware_counter = ll_timer16_get_tcnt(TIMER1_DEVICE);
    // tls->period_us = TICKLESS_PERIOD_US;
    tls->software_counter = z_tickless_counter;
    // tls->tick_us = TICKLESS_QUANTUM_US;

    irq_unlock(key);
}

void z_tickless_spec_convert(struct z_tickless_timespec *tls, struct timespec *ts)
{
    __ASSERT_NOTNULL(tls);
    __ASSERT_NOTNULL(ts);

    // FIX ME
    // ts->tv_sec = tls->software_counter * (tls->period_us / 1000000u);
    // ts->tv_msec = (tls->software_counter * (tls->period_us % 1000000u)) / 1000u;
}

#endif