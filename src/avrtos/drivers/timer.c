#include "timer.h"

#include <avrtos/assert.h>

#define K_MODULE K_MODULE_DRIVERS_TIMERS

/* not static inline version of timer_get_index */
static int _get_index(void *dev)
{
	return timer_get_index(dev);
}

static bool _is_8bits(uint8_t tim_idx)
{
	return IS_TIMER_IDX_8BITS(tim_idx);
}

static bool _is_16bits(uint8_t tim_idx)
{
	return IS_TIMER_IDX_16BITS(tim_idx);
}

void ll_timer8_drv_init(TIMER8_Device *dev,
			uint8_t tim_idx,
			const struct timer_config *config)
{
	/* clear prescaler (stop timer) */
	dev->TCCRnB = 0U;

	if (config->mode == TIMER_MODE_NORMAL) {
		dev->TCCRnA = 0U;
		dev->TCNTn = config->counter;
	} else if (config->mode == TIMER_MODE_CTC) {
		dev->OCRnA = config->counter & 0xFFU;
		dev->TCCRnA = BIT(WGMn1);
	}

	ll_timer_set_int_mask(tim_idx, config->timsk);

	/* timer starts counting from here  (prescaler != 0) */
	dev->TCCRnB = (config->prescaler << CSn0);
}

int timer8_drv_init(TIMER8_Device *dev,
		    const struct timer_config *config)
{
	if (config == NULL) {
		return -EINVAL;
	}

	int tim_idx = _get_index(dev);
	if (tim_idx < 0) {
		return tim_idx;
	}

	if (_is_8bits(tim_idx) == false) {
		return -EINVAL;
	}

	if ((tim_idx == 2) && (config->prescaler > TIMER_PRESCALER_1024)) {
		return -EINVAL;
	}

	/* timer starts counting when prescaler is different than 0 */
	ll_timer8_drv_init(dev, tim_idx, config);

	return 0;
}

void ll_timer16_drv_init(TIMER16_Device *dev,
			 uint8_t tim_idx,
			 const struct timer_config *config)
{
	dev->TCCRnB = 0U;
	dev->TCCRnA = 0U;
	dev->TCCRnC = 0U;

	if (config->mode == TIMER_MODE_NORMAL) {
		dev->TCNTn = config->counter;
	} else if (config->mode == TIMER_MODE_CTC) {
		/* read 16.9.2 Clear Timer on Compare Match (CTC) Mode */
		dev->OCRnA = config->counter;		
		dev->TCCRnB |= BIT(WGMn2);
	}

	ll_timer_set_int_mask(tim_idx, config->timsk);

	/* timer starts counting from here  (prescaler != 0) */
	dev->TCCRnB |= (config->prescaler << CSn0);
}

int timer16_drv_init(TIMER16_Device *dev,
		     const struct timer_config *config)
{
	if (config == NULL) {
		return -EINVAL;
	}

	int tim_idx = _get_index(dev);
	if (tim_idx < 0) {
		return tim_idx;
	}

	if (_is_16bits(tim_idx) == false) {
		return -EINVAL;
	}

	ll_timer16_drv_init(dev, tim_idx, config);

	return 0;
}


int timer8_drv_deinit(TIMER8_Device *dev)
{
	const int tim_idx = _get_index(dev);
	if (tim_idx < 0) {
		return tim_idx;
	}

	ll_timer_clear_int_mask(tim_idx);
	ll_timer_clear_irq_flags(tim_idx);

	dev->TCCRnB = 0U;
	dev->TCCRnA = 0U;

	dev->OCRnA = 0U;
	dev->OCRnB = 0U;
	dev->TCNTn = 0U;

	return 0;
}

int timer16_drv_deinit(TIMER16_Device *dev)
{
	const int tim_idx = _get_index(dev);
	if (tim_idx < 0) {
		return tim_idx;
	}

	ll_timer_clear_int_mask(tim_idx);
	ll_timer_clear_irq_flags(tim_idx);

		/* disable interrupts first */
	dev->TCCRnA = 0U;
	dev->TCCRnB = 0U;
	dev->TCCRnC = 0U;

	dev->OCRnA = 0U;
	dev->OCRnB = 0U;
	dev->OCRnC = 0U;
	dev->IRCN = 0U;
	dev->TCNTn = 0U;

	return 0;
}

#if DRIVERS_TIMERS_API

static void *_get_device(uint8_t tim_idx)
{
	return timer_get_device(tim_idx);
}

struct timer_api_ctx {
	timer_callback_t cb;
	void *user_data;

	uint8_t prescaler: 3U;
};

__attribute__((section(".bss"))) struct timer_api_ctx tim_ctx[TIMERS_COUNT];

#define __DECL_TIMER_COMPA_ISR(n) \
	ISR(TIMER##n##_COMPA_vect) \
	{ \
		struct timer_api_ctx *const ctx = &tim_ctx[n]; \
		void *const dev = timer_get_device(n); \
		if (ctx->cb != NULL) { \
			ctx->cb(dev, n, ctx->user_data); \
		} \
	}


#if DRIVERS_TIMER0_API && TIMER_INDEX_EXISTS(0)
__STATIC_ASSERT(KERNEL_SYSLOCK_HW_TIMER != 0, 
		"Cannot enable drivers API for system timer 0");
__DECL_TIMER_COMPA_ISR(0);
#endif /* DRIVERS_TIMER0_API */

#if DRIVERS_TIMER1_API && TIMER_INDEX_EXISTS(1)
__STATIC_ASSERT(KERNEL_SYSLOCK_HW_TIMER != 1, 
		"Cannot enable drivers API for system timer 1");
__DECL_TIMER_COMPA_ISR(1);
#endif /* DRIVERS_TIMER1_API */

#if DRIVERS_TIMER2_API && TIMER_INDEX_EXISTS(2)
__STATIC_ASSERT(KERNEL_SYSLOCK_HW_TIMER != 2, 
		"Cannot enable drivers API for system timer 2");
__DECL_TIMER_COMPA_ISR(2);
#endif /* DRIVERS_TIMER2_API */

#if DRIVERS_TIMER3_API && TIMER_INDEX_EXISTS(3)
__STATIC_ASSERT(KERNEL_SYSLOCK_HW_TIMER != 3, 
		"Cannot enable drivers API for system timer 3");
__DECL_TIMER_COMPA_ISR(3);
#endif /* DRIVERS_TIMER3_API */

#if DRIVERS_TIMER4_API && TIMER_INDEX_EXISTS(4)
__STATIC_ASSERT(KERNEL_SYSLOCK_HW_TIMER != 4, 
		"Cannot enable drivers API for system timer 4");
__DECL_TIMER_COMPA_ISR(4);
#endif /* DRIVERS_TIMER4_API */

#if DRIVERS_TIMER5_API && TIMER_INDEX_EXISTS(5)
__STATIC_ASSERT(KERNEL_SYSLOCK_HW_TIMER != 5, 
		"Cannot enable drivers API for system timer 5");
__DECL_TIMER_COMPA_ISR(5);
#endif /* DRIVERS_TIMER5_API */

static int get_timer_presc_value(timer_prescaler_t prescaler)
{
	switch (prescaler) {
	case TIMER_PRESCALER_1:
		return 1;
	case TIMER_PRESCALER_8:
		return 8;
	case TIMER_PRESCALER_64:
		return 64;
	case TIMER_PRESCALER_256:
		return 256;
	case TIMER_PRESCALER_1024:
		return 1024;
	default:
		return -EINVAL;
	}
}

static int get_timer2_presc_value(timer2_prescaler_t prescaler)
{
	switch (prescaler) {
	case TIMER2_PRESCALER_1:
		return 1;
	case TIMER2_PRESCALER_8:
		return 8;
	case TIMER2_PRESCALER_32:
		return 32;
	case TIMER2_PRESCALER_64:
		return 64;
	case TIMER2_PRESCALER_128:
		return 128;
	case TIMER2_PRESCALER_256:
		return 256;
	case TIMER2_PRESCALER_1024:
		return 1024;
	default:
		return -EINVAL;
	}
}

#define CALCULATE_COUNTER_VALUE(period_us, prescaler) ((((F_CPU / 1000000LU) * period_us) / prescaler - 1))
#define COUNTER_VALUE_FIT(period_us, prescaler, max) (CALCULATE_COUNTER_VALUE(period_us, prescaler) <= max)

int timer_init(uint8_t tim_idx,
	       uint32_t period_us,
	       timer_callback_t cb,
	       void *user_data,
	       uint8_t flags)
{
	if ((tim_idx == KERNEL_SYSLOCK_HW_TIMER) || (cb == NULL)) {
		return -EINVAL;
	}

	void *const dev = _get_device(tim_idx);
	if (dev == NULL) {
		return -EINVAL;
	}

	const bool is_16bit = _is_16bits(tim_idx);
	const uint16_t max_counter = is_16bit ? 0xFFFFU : 0xFFU;
	const bool is_timer2 = tim_idx == 2; /* timer2 has more prescaler values */

	/* iterate over prescaler values and find the best match */
	uint8_t prescaler_id = 0;
	uint32_t counter;

	do {
		prescaler_id++; /* fetch next prescaler */
		int prescaler_val = is_timer2 ?
			get_timer2_presc_value(prescaler_id) :
			get_timer_presc_value(prescaler_id);
		if (prescaler_val < 0) {
			return -1;
		}

		counter = CALCULATE_COUNTER_VALUE(period_us, prescaler_val);
	} while (counter > max_counter);

	tim_ctx[tim_idx].cb = cb;
	tim_ctx[tim_idx].user_data = user_data;
	tim_ctx[tim_idx].prescaler = prescaler_id;

	const struct timer_config cfg = {
		.mode = TIMER_MODE_CTC,
		.prescaler = (flags & TIMER_API_FLAG_AUTOSTART) ? prescaler_id : 0U,
		.counter = counter,
		.timsk = BIT(OCIEnA)
	};

	if (is_16bit) {
		ll_timer16_drv_init((TIMER16_Device *)dev, tim_idx, &cfg);
	} else {
		ll_timer8_drv_init((TIMER8_Device *)dev, tim_idx, &cfg);
	}

	return 0;
}

void timer_start(uint8_t tim_idx)
{
	__ASSERT_TRUE(TIMER_INDEX_EXISTS(tim_idx));

	void *const dev = _get_device(tim_idx);
	if (dev != NULL) {
		ll_timer_start(dev, timer_get_prescaler(tim_idx));
	}
}

void timer_stop(uint8_t tim_idx)
{
	__ASSERT_TRUE(TIMER_INDEX_EXISTS(tim_idx));

	void *const dev = _get_device(tim_idx);
	if (dev != NULL) {
		ll_timer_stop(dev);
	}
}

uint8_t timer_get_prescaler(uint8_t tim_idx)
{
	__ASSERT_TRUE(TIMER_INDEX_EXISTS(tim_idx));

	return tim_ctx[tim_idx].prescaler;
}

uint32_t timer_get_max_period_us(uint8_t tim_idx)
{
	if (_is_16bits(tim_idx)) {
		return (1024LU * 0x10000LU) / (F_CPU / 1000000LU);
	} else if (_is_8bits(tim_idx)) {
		return (1024LU * 0x100LU) / (F_CPU / 1000000LU);
	} else {
		return 0;
	}
}

#endif /* DRIVERS_TIMERS_API */