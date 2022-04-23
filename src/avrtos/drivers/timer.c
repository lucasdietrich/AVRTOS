#include "timer.h"

void ll_timer8_drv_init(TIMER8_Device *dev,
		       const struct timer_config *config)
{
	if (config->mode == TIMER_MODE_NORMAL) {
	
	} else if (config->mode == TIMER_MODE_CTC) {
		SET_BIT(dev->TCCRnA, BIT(WGMn1));
		dev->OCRnA = config->counter & 0xFFU;
		dev->TCCRnB |= (config->prescaler << CSn0);
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
	if (config->mode == TIMER_MODE_NORMAL) {
	
	} else if (config->mode == TIMER_MODE_CTC) {
		/* read 16.9.2 Clear Timer on Compare Match (CTC) Mode */
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