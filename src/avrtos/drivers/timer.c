#include "timer.h"

int timer_get_index(void *dev)
{
	int ret = -EINVAL;

	if (IS_TIMER0_DEVICE(dev)) {
		ret = 0;
	} else if (IS_TIMER1_DEVICE(dev)) {
		ret = 1;
	} else if (IS_TIMER2_DEVICE(dev)) {
		ret = 2;
	} else if (IS_TIMER3_DEVICE(dev)) {
		ret = 3;
	} else if (IS_TIMER4_DEVICE(dev)) {
		ret = 4;
	} else if (IS_TIMER5_DEVICE(dev)) {
		ret = 5;
	}

	return  ret;
}

void ll_timer8_drv_init(TIMER8_Device *dev,
		       const struct timer_config *config)
{
	if (config->mode == TIMER_MODE_CTC) {
		
	}
}

int timer8_drv_init(TIMER8_Device *dev,
		    const struct timer_config *config)
{
	if ((dev == NULL) || (config == NULL)) {
		return -EINVAL;
	}

	ll_timer8_drv_init(dev, config);

	return 0;
}

void ll_timer16_drv_init(TIMER16_Device *dev,
		       const struct timer_config *config)
{
	if (config->mode == TIMER_MODE_CTC) {
		SET_BIT(dev->TCCRnB, BIT(WGMn2));
		dev->OCRnA = config->counter;
		dev->TCCRnB |= (config->prescaler << CSn0);
	}
}

int timer16_drv_init(TIMER16_Device *dev,
		     const struct timer_config *config)
{
	if ((dev == NULL) || (config == NULL)) {
		return -EINVAL;
	}

	ll_timer16_drv_init(dev, config);

	return 0;
}

int timer8_drv_deinit(TIMER8_Device *dev,
		      const struct timer_config *config)
{
	return -EINVAL;
}

int timer16_drv_deinit(TIMER16_Device *dev,
		       const struct timer_config *config)
{
	return -EINVAL;
}