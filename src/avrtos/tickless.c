#include "tickless.h"
#include <stdint.h>
#include <stdio.h>

#include <avr/iom2560.h>
#include <avr/pgmspace.h>

#include "avrtos/assert.h"
#include "avrtos/kernel.h"
#include "avrtos/sys.h"
#include "drivers/timer.h"
#include "gpio.h"
#include "kernel.h"
#include "serial.h"
#include "timer_defs.h"

#define LOG_LEVEL LOG_LEVEL_DBG
#define K_MODULE  K_MODULE_TICKLESS_CLOCK

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

#define TICKLESS_MODE TICKLESS_MODE_PRECISION

#if CONFIG_KERNEL_TICKLESS

#if 0
#warning "Using prescaler 256 for tickless mode, this will result in a 16us tick"
// prescaler = 256  freq = 0.95367431640625 Hz (16.0 us)  period = 1048576 us
#define TICKLESS_PRESCALER       TIMER_PRESCALER_256
#define TICKLESS_PRESCALER_VALUE 256u
#define TICKLESS_US_PER_TICK     16u
#define TICKLESS_US_PER_PERIOD   1048576u
#define TICKLESS_TOP_PERIOD_US   1000000u
#elif 0
#warning "Using prescaler 64 for tickless mode, this will result in a 4us tick"
// prescaler = 64  freq = 3.814697265625 Hz (4.0 us)  period = 262144 us
#define TICKLESS_PRESCALER       TIMER_PRESCALER_64
#define TICKLESS_PRESCALER_VALUE 64u
#define TICKLESS_US_PER_TICK     4u
#define TICKLESS_US_PER_PERIOD   262144u
#define TICKLESS_TOP_PERIOD_US   250000u
#else
#warning "Using prescaler 8 for tickless mode, this will result in a 0.5 us tick"
// prescaler = 8  freq = 7812.5 Hz (0.5 us)  period = 128 us
#define TICKLESS_PRESCALER       TIMER_PRESCALER_8
#define TICKLESS_PRESCALER_VALUE 8u
#define TICKLESS_US_PER_TICK     0.5
#define TICKLESS_US_PER_PERIOD   32768u
#define TICKLESS_TOP_PERIOD_US   25000u
#endif

#if TICKLESS_MODE
#define TICKLESS_TOP_TICK_VALUE                                                          \
    TIMER_CALC_COUNTER_VALUE(TICKLESS_TOP_PERIOD_US, TICKLESS_PRESCALER_VALUE)
#define TICKLESS_TOP_TICK_VALUE_COMPLEMENT (65535u - TICKLESS_TOP_TICK_VALUE)
#define TICKLESS_PERIOD_US                 TICKLESS_TOP_PERIOD_US
#else
#define TICKLESS_TOP_TICK_VALUE 0xFFFFu
#define TICKLESS_PERIOD_US      TICKLESS_US_PER_PERIOD
#endif /* TICKLESS_MODE */

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

// FIXME this variable might not be required, but could be deduced from other variables
static enum tickless_state z_tickless_state = TICKLESS_IDLE;

static uint16_t z_tickless_unwrap_counter = 0u;

ISR(TIMER1_COMPA_vect)
{
#if CONFIG_KERNEL_TICKLESS_DEBUG
    gpiol_pin_write_state(GPIOF, 5u, 0u);
    // serial_transmit('k');
#endif

    // Disable the interrupt as the event is serviced
    z_tickless_sched_cancel();
}

// ISR(TIMER1_COMPB_vect)
// {
//     gpiol_pin_toggle(GPIOF, 5u);
// }

// ISR(TIMER1_COMPC_vect)
// {
//     gpiol_pin_toggle(GPIOF, 6u);
// }

#if TICKLESS_MODE
ISR(TIMER1_CAPT_vect)
#else
ISR(TIMER1_OVF_vect)
#endif
{
#if CONFIG_KERNEL_TICKLESS_DEBUG
    gpiol_pin_toggle(GPIOF, 7u);
    // serial_transmit('o');
#endif

    z_tickless_counter += 1u;

    // If we are in the far scheduling state, we need to decrement the
    // unwrap counter, which is used to track how many overflows we need
    // to wait before we can schedule the next tickless event.
    // - If the unwrap counter is 0, we can schedule the next tickless event.
    // - If the unwrap counter is greater than 0, we just decrement it and return.
    if (z_tickless_state == TICKLESS_FAR_SCHEDULING) {

        // printf_P(PSTR("z_tickless_unwrap_counter: %u\n"), z_tickless_unwrap_counter);
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
#if TICKLESS_MODE
    const struct timer_config config = {
        .mode      = TIMER_MODE_CTC_ICRn,
        .counter   = TICKLESS_TOP_TICK_VALUE,
        .prescaler = TICKLESS_PRESCALER,
        .timsk     = BIT(ICIEn), // BIT(TOIEn) // BIT(OCIEnA)  | BIT(OCIEnB) | BIT(OCIEnC)
    };
    printf_P(PSTR("Tickless mode precision (CTC_IRCn), prescaler: %u, top: %u\n"),
             TICKLESS_PRESCALER, TICKLESS_TOP_TICK_VALUE);
#else
    const struct timer_config config = {
        .mode      = TIMER_MODE_NORMAL,
        .counter   = 0u,
        .prescaler = TICKLESS_PRESCALER,
        .timsk     = BIT(TOIEn), // BIT(TOIEn) // BIT(OCIEnA)  | BIT(OCIEnB) | BIT(OCIEnC)
    };
    printf_P(PSTR("Tickless mode performance (NORMAL), prescaler: %u\n"),
             TICKLESS_PRESCALER);
#endif

    gpiol_pin_init(GPIOF, 4u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 5u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 6u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 7u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);

    ll_timer16_channel_set_mode(TIMER1_DEVICE, TIMER_CHANNEL_A,
                                TIMER_CHANNEL_COMP_MODE_NORMAL);
    ll_timer16_channel_set_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A, 0u);
    ll_timer16_init(TIMER1_DEVICE, timer_get_index(TIMER1_DEVICE), &config);
}

static void z_tickless_sched_ms_inner(uint16_t loops, uint16_t offset, uint8_t new_ref);

void z_tickless_sched_ms(uint32_t ms)
{
    uint16_t loops;
    uint16_t offset;

    // FIXME remove
    ms -= 1u;

    // 1ms with 64us tickless quantum becomes is 15.625 ticks which becomes 16 ticks
    // FIXME: not sure about the +1u.
    uint32_t ticks = (ms * 1000u) / TICKLESS_US_PER_TICK; // + 1u ?

// use timer full scale (0xFFFFu) amplitude
#if TICKLESS_MODE
    // Use partial amplitude (ICR1)
    offset = ticks % (TICKLESS_TOP_TICK_VALUE + 1u); // Get the offset in the range of
                                                     // 0..TICKLESS_TOP_TICK_VALUE-1
    loops = ticks / (TICKLESS_TOP_TICK_VALUE +
                     1u); // Get the number of loops in the range of 0..65535
    // Try to use __udivmodhi4.
#else
    offset  = (uint16_t)ticks; // Get the offset in the range of 0..65535
    loops   = ticks >> 16u;
#endif

    z_tickless_sched_ms_inner(loops, offset, true);
}

static __always_inline uint16_t z_tickless_calc_new_comp_ref(uint16_t counter,
                                                             uint16_t offset)
{
#if (TICKLESS_TOP_TICK_VALUE) != 0xFFFFu
    /* Ensure that the addition of offset to tcnt does not wrap around 16-bit counter
     * Calculate 65536 - offset, which is the maximum value
     * that can be added to TCNT without wrapping around
     */
    const uint16_t safe_max_add = (uint16_t)(-offset);
    if (counter >= safe_max_add) {
        /* If adding offset would wrap around, split the operation to avoid overflow
         *
         * Equivalent to:
         *   (tcnt + offset) % (TICKLESS_TOP_TICK_VALUE + 1)
         *
         * But performed as:
         *   (tcnt - (65536 - offset)) + (65536 - TICKLESS_TOP_TICK_VALUE + 1)
         *
         * Which avoids overflow
         */
        counter = counter - safe_max_add;
        counter = counter + TICKLESS_TOP_TICK_VALUE_COMPLEMENT;
    } else {
        /* Safe to add directly without wraparound */
        counter = (counter + offset) % (TICKLESS_TOP_TICK_VALUE + 1u);
    }
#else
    counter = (counter + offset);
#endif

    return counter;
}

__always_inline static void
z_tickless_sched_ms_inner(uint16_t loops, uint16_t offset, uint8_t new_ref)
{
    __ASSERT_NOINTERRUPT();

    // gpiol_pin_write_state(GPIOF, 6u, 1u);

    gpiol_pin_write_state(GPIOF, 5u, 1u);

    uint16_t new_counter, counter;
    if (new_ref) {
        counter = ll_timer16_get_tcnt(TIMER1_DEVICE);
    } else {
        counter = ll_timer16_channel_get_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A);
    }

    new_counter = z_tickless_calc_new_comp_ref(counter, offset);
    if (new_counter < counter) {
        loops += 1u;
        gpiol_pin_toggle(GPIOF, 6u);
    }

    /* Make sure to test the case when the new counter equals to 0,
     * which means that the TIMER1_COMPA_vect irq will be enabled and triggered
     * during the execution of the last TIMER1_OVF_vect irq handler.
     * It could be possible to delay the triggering of the TIMER1_COMPA_vect
     * until the next TIMER1_OVF_vect irq handler if its execution is too long.
     */
    ll_timer16_clear_irq_flag(TIMER1_INDEX, OCIEnA);
    ll_timer16_channel_set_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A, new_counter);

    if (loops) {
        // If the ticks are more than 65536, we cannot schedule it in a single
        // timer compare match register, so we will schedule it in the next
        // overflow event.
        z_tickless_unwrap_counter = loops - 1u;
        z_tickless_state          = TICKLESS_FAR_SCHEDULING;
    } else {
        ll_timer16_enable_interrupt(TIMER1_INDEX, OCIEnA);
        z_tickless_state = TICKLESS_SCHEDULED;
    }

    // gpiol_pin_write_state(GPIOF, 6u, 0u);
}

void z_tickless_continue_ms(uint32_t ms)
{
    uint16_t loops;
    uint16_t offset;

    // FIXME remove
    ms -= 1u;

    // 1ms with 64us tickless quantum becomes is 15.625 ticks which becomes 16 ticks
    // FIXME: not sure about the +1u.
    uint32_t ticks = (ms * 1000u) / TICKLESS_US_PER_TICK; // + 1u ?

// use timer full scale (0xFFFFu) amplitude
#if TICKLESS_MODE
    // Use partial amplitude (ICR1)
    offset = ticks % (TICKLESS_TOP_TICK_VALUE + 1u); // Get the offset in the range of
                                                     // 0..TICKLESS_TOP_TICK_VALUE-1
    loops = ticks / (TICKLESS_TOP_TICK_VALUE +
                     1u); // Get the number of loops in the range of 0..65535
    // Try to use __udivmodhi4.
#else
    offset  = (uint16_t)ticks; // Get the offset in the range of 0..65535
    loops   = ticks >> 16u;
#endif

    z_tickless_sched_ms_inner(loops, offset, false);
}

void z_tickless_sched_cancel(void)
{
    // printf_P(PSTR("z_tickless_sched_cancel()\n"));
    // __ASSERT_NOINTERRUPT();
    ll_timer16_disable_interrupt(TIMER1_INDEX, OCIEnA);
    z_tickless_state = TICKLESS_IDLE;
}

void z_tickless_time_get(struct z_tickless_timespec *tls)
{
    __ASSERT_NOTNULL(tls);

    uint8_t key = irq_lock();

    tls->hardware_counter = ll_timer16_get_tcnt(TIMER1_DEVICE);
    tls->global_counter   = z_tickless_counter;

    irq_unlock(key);
}

__noinline void z_tickless_spec_convert(struct z_tickless_timespec *tls,
                                        struct timespec *ts)
{
    __ASSERT_NOTNULL(tls);
    __ASSERT_NOTNULL(ts);

    // FIXME
    // Prefer 40-bit arithmetic instead of 64-bit.
    uint64_t total_ms = ((uint64_t)tls->global_counter * TICKLESS_PERIOD_US) / 1000u +
                        ((uint64_t)tls->hardware_counter * TICKLESS_US_PER_TICK) / 1000u;

    ts->tv_sec  = total_ms / 1000u; // Convert milliseconds to seconds
    ts->tv_msec = total_ms % 1000u; // Get the remaining milliseconds
}

#endif