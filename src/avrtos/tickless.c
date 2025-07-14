#include "tickless.h"
#include <stdint.h>
#include <stdio.h>

#include <avr/iom2560.h>
#include <avr/pgmspace.h>

#include "avrtos/assert.h"
#include "avrtos/fault.h"
#include "avrtos/kernel.h"
#include "avrtos/sys.h"
#include "drivers/timer.h"
#include "gpio.h"
#include "kernel.h"
#include "serial.h"
#include "timer_defs.h"
#include "tickless_defs.h"

#define LOG_LEVEL LOG_LEVEL_DBG
#define K_MODULE  K_MODULE_TICKLESS_CLOCK

#include "systime.h"

#if CONFIG_KERNEL_TICKLESS

#if TICKLESS_PRESCALER_VALUE == 1024
#warning "Using prescaler 1024 for tickless mode, this will result in a 64us tick"
#define TICKLESS_PRESCALER       TIMER_PRESCALER_1024
#elif TICKLESS_PRESCALER_VALUE == 256
#warning "Using prescaler 256 for tickless mode, this will result in a 16us tick"
#define TICKLESS_PRESCALER       TIMER_PRESCALER_256
#elif TICKLESS_PRESCALER_VALUE == 64
#warning "Using prescaler 64 for tickless mode, this will result in a 4us tick"
#define TICKLESS_PRESCALER       TIMER_PRESCALER_64
#elif TICKLESS_PRESCALER_VALUE == 8
#warning "Using prescaler 8 for tickless mode, this will result in a 0.5 us tick"
#define TICKLESS_PRESCALER       TIMER_PRESCALER_8
#elif TICKLESS_PRESCALER_VALUE == 1
#warning "Using prescaler 1 for tickless mode, this will result in a 0.0625 us tick"
#define TICKLESS_PRESCALER       TIMER_PRESCALER_1
#else
#error "Unsupported TICKLESS_PRESCALER_VALUE, must be 1, 8, 64, 256 or 1024"
#endif

#if TICKLESS_WRAP_MODE == TICKLESS_WRAP_MODE_PRECISION
#define TICKLESS_TOP_TICK_VALUE                                                          \
    TIMER_CALC_COUNTER_VALUE(TICKLESS_TOP_PERIOD_US, TICKLESS_PRESCALER_VALUE)
#define TICKLESS_TOP_TICK_VALUE_COMPLEMENT (65535u - TICKLESS_TOP_TICK_VALUE)
#define TICKLESS_PERIOD_US                 TICKLESS_TOP_PERIOD_US
#else
#define TICKLESS_TOP_TICK_VALUE 0xFFFFu
#define TICKLESS_PERIOD_US      TICKLESS_US_PER_PERIOD
#endif /* TICKLESS_WRAP_MODE */

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

// ISR(TIMER1_COMPA_vect)
// {
// #if CONFIG_KERNEL_TICKLESS_DEBUG
//     gpiol_pin_write_state(GPIOF, 1u, 0u);
//     // serial_transmit('k');
// #endif

//     // Disable the interrupt as the event is serviced
//     z_tickless_sched_cancel();
// }

// ISR(TIMER1_COMPB_vect)
// {
//     gpiol_pin_toggle(GPIOF, 1u);
// }

// ISR(TIMER1_COMPC_vect)
// {
//     gpiol_pin_toggle(GPIOF, 2u);
// }

#if TICKLESS_WRAP_MODE == TICKLESS_WRAP_MODE_PRECISION
ISR(TIMER1_CAPT_vect)
#else
ISR(TIMER1_OVF_vect)
#endif
{
    #if CONFIG_KERNEL_TICKLESS_DEBUG
    gpiol_pin_toggle(GPIOF, 0u);
    // serial_transmit('o');
    #endif

    z_tickless_counter += 1u;

    // If we are in the far scheduling state, we need to decrement the
    // unwrap counter, which is used to track how many overflows we need
    // to wait before we can schedule the next tickless event.
    // - If the unwrap counter is 0, we can schedule the next tickless event.
    // - If the unwrap counter is greater than 0, we just decrement it and return.
    
    if (z_ker.tickless.flags & Z_TICKLESS_FAR_SCHEDULING) {
        z_ker.tickless.elapsed_loops += 1u;

        // printf_P(PSTR("z_tickless_unwrap_counter: %u\n"), z_tickless_unwrap_counter);
        if (z_ker.tickless.unwrap_counter > 0u) {
            z_ker.tickless.unwrap_counter -= 1u;
            return;
        }

        // We can now schedule the next tickless event
        ll_timer16_clear_irq_flag(TIMER1_INDEX, TIMER16_OUTPUT_COMPARE_MATCH_A_INTERRUPT);
        ll_timer16_enable_interrupt(TIMER1_INDEX, TIMER16_OUTPUT_COMPARE_MATCH_A_INTERRUPT);
        z_ker.tickless.flags &= ~Z_TICKLESS_FAR_SCHEDULING;
    }
}

void z_tickless_init(void)
{
#if TICKLESS_WRAP_MODE == TICKLESS_WRAP_MODE_PRECISION
    const struct timer_config config = {
        .mode      = TIMER_MODE_CTC_ICRn,
        .counter   = TICKLESS_TOP_TICK_VALUE,
        .prescaler = TICKLESS_PRESCALER,
        .timsk     = BIT(ICIEn), // BIT(TOIEn) // BIT(OCIEnA)  | BIT(OCIEnB) | BIT(OCIEnC)
    };
    printf_P(PSTR("Tickless mode precision (CTC_IRCn), prescaler: %u, top: %u\n"),
             TICKLESS_PRESCALER_VALUE, TICKLESS_TOP_TICK_VALUE);
#else
    const struct timer_config config = {
        .mode      = TIMER_MODE_NORMAL,
        .counter   = 0u,
        .prescaler = TICKLESS_PRESCALER,
        .timsk     = BIT(TOIEn), // BIT(TOIEn) // BIT(OCIEnA)  | BIT(OCIEnB) | BIT(OCIEnC)
    };
    printf_P(PSTR("Tickless mode performance (NORMAL), prescaler: %u\n"),
             TICKLESS_PRESCALER_VALUE);
#endif

    printf_P(PSTR("Z_TICK_US: %u us, TICKLESS_PERIOD_US: %u us\n"),
             Z_TICK_US, TICKLESS_PERIOD_US);

    gpiol_pin_init(GPIOF, 0u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 1u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 2u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);
    gpiol_pin_init(GPIOF, 3u, GPIO_MODE_OUTPUT, GPIO_OUTPUT_DRIVEN_LOW);

    
    ll_timer16_channel_set_mode(TIMER1_DEVICE, TIMER_CHANNEL_A,
                                TIMER_CHANNEL_COMP_MODE_NORMAL);
    // unnecessary
    // ll_timer16_channel_set_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A, 0u);
    ll_timer16_init(TIMER1_DEVICE, timer_get_index(TIMER1_DEVICE), &config);
}

// TODO arguments loops and offset might not be necessary, we could use z_ker.tickless.sp_loops and .sp_ticks
//       to store the number of loops and the offset, respectively, this saves 4B on stack
static void z_tickless_set_sched_point(uint16_t loops, uint16_t offset, SP_TYPE type);

void z_tickless_sched_ticks(uint32_t ticks, SP_TYPE sp_type)
{
    if (!ticks) {
        __fault(K_FAULT_TIMING);
    }

    uint16_t loops;
    uint16_t offset;

// use timer full scale (0xFFFFu) amplitude
#if TICKLESS_WRAP_MODE == TICKLESS_WRAP_MODE_PRECISION
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

    z_tickless_set_sched_point(loops, offset, sp_type);
}

static __always_inline uint16_t z_tickless_counter_add_mod(uint16_t counter,
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
         *   (tcnt - (65536 - offset)) + (65536 - (TICKLESS_TOP_TICK_VALUE + 1))
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
    counter = counter + offset;
#endif

    return counter;
}

static __always_inline uint16_t z_tickless_counter_sub_mod(uint16_t counter,
                                                           uint16_t offset)
{
#if (TICKLESS_TOP_TICK_VALUE) != 0xFFFFu
    /* Ensure that the substraction of offset to tcnt does not wrap around 16-bit counter
     */
    if (counter < offset) {
        /* If subtracting offset would wrap around, split the operation to avoid overflow
         *
         * Equivalent to:
         *   (tcnt - offset) % (TICKLESS_TOP_TICK_VALUE + 1)
         *
         * But performed as:
         *   (TICKLESS_TOP_TICK_VALUE + 1) - (offset - tcnt)
         *
         * Which avoids overflow
         */
        counter = (TICKLESS_TOP_TICK_VALUE + 1u) - (offset - counter);
    } else {
        /* Safe to subtract directly without wraparound */
        counter = (counter - offset);
    }
#else
    counter = counter - offset;
#endif

    return counter;
}


static void z_tickless_set_sched_point(uint16_t loops, uint16_t offset, SP_TYPE type)
{
    __ASSERT_NOINTERRUPT();

    // printf("l: %u o: %u\n", loops, offset);

    gpiol_pin_toggle(GPIOF, 2u);
    // gpiol_pin_write_state(GPIOF, 2 u, 1u);

    uint16_t counter, new_counter;
    switch (type) {
        case SP_NEW_POINT:
            counter = ll_timer16_get_tcnt(TIMER1_DEVICE);
            break;
        case SP_CONTINUE_LAST:
            counter = z_ker.tickless.last_sp_compa;
            // FIXME this is probably required, what to do if elapsed loops < elapsed loops ?
            // loops -= z_ker.tickless.elapsed_loops;
            break;
        case SP_CONTINUE_NEXT:
            counter = ll_timer16_channel_get_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A);
            // FIXME this is probably required, what to do if elapsed loops < elapsed loops ?
            // loops -= z_ker.tickless.elapsed_loops;
            break;
        default:
            panic();
    }

    new_counter = z_tickless_counter_add_mod(counter, offset);
    if (new_counter < counter) {
        loops += 1u;
    }

    // Store last counter
    z_ker.tickless.last_sp_compa = counter;

    // TODO this late value set might not be efficient
    // z_ker.tickless.sp_loops = loops;

    /* Make sure to test the case when the new counter equals to 0,
     * which means that the TIMER1_COMPA_vect irq will be enabled and triggered
     * during the execution of the last TIMER1_OVF_vect irq handler.
     * It could be possible to delay the triggering of the TIMER1_COMPA_vect
     * until the next TIMER1_OVF_vect irq handler if its execution is too long.
     */
    // serial_hex(TIFRn[TIMER1_INDEX]);
    ll_timer16_clear_irq_flag(TIMER1_INDEX, TIMER16_OUTPUT_COMPARE_MATCH_A_INTERRUPT);
    ll_timer16_channel_set_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A, new_counter);

    if (loops) {
        // If the ticks are more than 65536, we cannot schedule it in a single
        // timer compare match register, so we will schedule it in the next
        // overflow event.
        z_ker.tickless.unwrap_counter = loops - 1u;
        z_ker.tickless.elapsed_loops = 0u;
        z_ker.tickless.flags |= Z_TICKLESS_FAR_SCHEDULING;
    } else {
        ll_timer16_enable_interrupt(TIMER1_INDEX, TIMER16_OUTPUT_COMPARE_MATCH_A_INTERRUPT);
        z_ker.tickless.elapsed_loops = 0u;
        z_ker.tickless.flags &= ~Z_TICKLESS_FAR_SCHEDULING;
    }

    // gpiol_pin_write_state(GPIOF, 2u, 0u);
}

uint32_t z_tickless_early_sp(bool advance_sp)
{
// #error "Error when the scheduled delays exceed the timer overflow period !!!"

    // Get the current timer counter value
    uint16_t tcnt = ll_timer16_get_tcnt(TIMER1_DEVICE);

    // Get the current compare match register value
    // uint16_t comp_a = ll_timer16_channel_get_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A);
    uint16_t comp_a = z_ker.tickless.last_sp_compa;

    // Calculate Et = (TCNT - 0_A) % (TICKLESS_TOP_TICK_VALUE + 1u)
    // TODO verify sign
    uint16_t elapsed_ticks = z_tickless_counter_sub_mod(tcnt, comp_a);

    if (advance_sp) {
        // Advance COMPA to current TCNT
        ll_timer16_channel_set_compare_register(TIMER1_DEVICE, TIMER_CHANNEL_A, tcnt);
    }

    // Get the number of loops ellapsed since the last tickless event
    uint16_t elapsed_loops = z_ker.tickless.elapsed_loops;

    // FIXME
    // elapsed_loops -= 1;

#if TICKLESS_TOP_TICK_VALUE == 0xFFFFu
    uint32_t ticks = ((uint32_t)elapsed_loops << 16u) + elapsed_ticks;
#else
    uint32_t ticks = elapsed_ticks + ((uint32_t)elapsed_loops * (TICKLESS_TOP_TICK_VALUE + 1llu));
#endif

    return ticks;
}

void z_tickless_sched_cancel(void)
{
    // printf_P(PSTR("z_tickless_sched_cancel()\n"));
    // __ASSERT_NOINTERRUPT();
    ll_timer16_disable_interrupt(TIMER1_INDEX, TIMER16_OUTPUT_COMPARE_MATCH_A_INTERRUPT);
    z_ker.tickless.flags &= ~Z_TICKLESS_FAR_SCHEDULING;
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