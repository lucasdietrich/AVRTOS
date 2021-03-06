#include <avrtos/defines.h>
#include <avrtos/drivers/timer.h>

#if (KERNEL_SYSCLOCK_PERIOD_US < 100) 
#	warning SYSCLOCK is probably too fast !
#endif 

#if (KERNEL_TIME_SLICE_US < 500)
# 	warning KERNEL_TIME_SLICE_US is probably too short !
#endif 

#if KERNEL_SYSLOCK_HW_TIMER >= TIMERS_COUNT
#	error "invalid KERNEL_SYSLOCK_HW_TIMER"
#endif

#define TIMER_MAX_COUNTER TIMER_GET_MAX_COUNTER(KERNEL_SYSLOCK_HW_TIMER)

#if TIMER_COUNTER_VALUE_FIT(KERNEL_SYSCLOCK_PERIOD_US, 1LU, TIMER_MAX_COUNTER)
#	define PRESCALER_VALUE 1
#elif TIMER_COUNTER_VALUE_FIT(KERNEL_SYSCLOCK_PERIOD_US, 8LU, TIMER_MAX_COUNTER)
#	define PRESCALER_VALUE 8
#elif TIMER_COUNTER_VALUE_FIT(KERNEL_SYSCLOCK_PERIOD_US, 32LU, TIMER_MAX_COUNTER) && (KERNEL_SYSLOCK_HW_TIMER == 2)
#	define PRESCALER_VALUE 32
#elif TIMER_COUNTER_VALUE_FIT(KERNEL_SYSCLOCK_PERIOD_US, 64LU, TIMER_MAX_COUNTER)
#	define PRESCALER_VALUE 64
#elif TIMER_COUNTER_VALUE_FIT(KERNEL_SYSCLOCK_PERIOD_US, 128LU, TIMER_MAX_COUNTER) && (KERNEL_SYSLOCK_HW_TIMER == 2)
#	define PRESCALER_VALUE 128
#elif TIMER_COUNTER_VALUE_FIT(KERNEL_SYSCLOCK_PERIOD_US, 256LU, TIMER_MAX_COUNTER)
#	define PRESCALER_VALUE 256
#elif TIMER_COUNTER_VALUE_FIT(KERNEL_SYSCLOCK_PERIOD_US, 1024LU, TIMER_MAX_COUNTER)
#	define PRESCALER_VALUE 1024
#else
#	error "KERNEL_SYSCLOCK_PERIOD_US is too big for the selected timer"
#endif


#if KERNEL_SYSLOCK_HW_TIMER == 2

#	if PRESCALER_VALUE == 1
#		define PRESCALER_CONFIG TIMER2_PRESCALER_1
#	elif PRESCALER_VALUE == 8
#		define PRESCALER_CONFIG TIMER2_PRESCALER_8
#	elif PRESCALER_VALUE == 32
#		define PRESCALER_CONFIG TIMER2_PRESCALER_32
#	elif PRESCALER_VALUE == 64
#		define PRESCALER_CONFIG TIMER2_PRESCALER_64
#	elif PRESCALER_VALUE == 128
#		define PRESCALER_CONFIG TIMER2_PRESCALER_128
#	elif PRESCALER_VALUE == 256
#		define PRESCALER_CONFIG TIMER2_PRESCALER_256
#	elif PRESCALER_VALUE == 1024
#		define PRESCALER_CONFIG TIMER2_PRESCALER_1024
#	else
#		error "invalid PRESCALER_VALUE (timer 2)"
#	endif

#else

#	if PRESCALER_VALUE == 1
#		define PRESCALER_CONFIG TIMER_PRESCALER_1
#	elif PRESCALER_VALUE == 8
#		define PRESCALER_CONFIG TIMER_PRESCALER_8
#	elif PRESCALER_VALUE == 64
#		define PRESCALER_CONFIG TIMER_PRESCALER_64
#	elif PRESCALER_VALUE == 256
#		define PRESCALER_CONFIG TIMER_PRESCALER_256
#	elif PRESCALER_VALUE == 1024
#		define PRESCALER_CONFIG TIMER_PRESCALER_1024
#	else
#		error "invalid PRESCALER_VALUE (timers 1, 3, 4, 5)"
#	endif

#endif /* KERNEL_SYSLOCK_HW_TIMER == 2 */


#if (F_CPU * KERNEL_SYSCLOCK_PERIOD_US) % (PRESCALER_VALUE * 1000000LU) != 0
#	warning "Sysclock may not be accurate"
#endif

#define COUNTER_VALUE TIMER_CALC_COUNTER_VALUE(KERNEL_SYSCLOCK_PERIOD_US, PRESCALER_VALUE)


void _k_init_sysclock(void)
{
	void *const dev = timer_get_device(KERNEL_SYSLOCK_HW_TIMER);

	const struct timer_config cfg = {
		.mode = TIMER_MODE_CTC,
		.prescaler = PRESCALER_CONFIG,
		.counter = COUNTER_VALUE,
		.timsk = BIT(OCIEnA),
	};

#if IS_TIMER_INDEX_16BIT(KERNEL_SYSLOCK_HW_TIMER)
	ll_timer16_drv_init(dev, KERNEL_SYSLOCK_HW_TIMER, &cfg);
#elif IS_TIMER_INDEX_8BIT(KERNEL_SYSLOCK_HW_TIMER)
	ll_timer8_drv_init(dev, KERNEL_SYSLOCK_HW_TIMER, &cfg);
#else
#	error "invalid timer type"
#endif
}