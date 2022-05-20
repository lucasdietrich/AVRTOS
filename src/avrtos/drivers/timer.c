#include "timer.h"

void ll_timer8_drv_init(TIMER8_Device *dev,
			const struct timer_config *config)
{
	dev->TCCRnB = (config->prescaler << CSn0);

	if (config->mode == TIMER_MODE_NORMAL) {
		dev->TCCRnA = 0U;
		dev->TCNTn = config->counter;
	} else if (config->mode == TIMER_MODE_CTC) {
		dev->TCCRnA = BIT(WGMn1);
		dev->OCRnA = config->counter & 0xFFU;
	}
}

int timer8_drv_init(TIMER8_Device *dev,
		    const struct timer_config *config)
{
	if ((dev == NULL) || (config == NULL)) {
		return -EINVAL;
	}

	if (IS_TIMER2_DEVICE(dev) && (config->prescaler > TIMER_PRESCALER_1024)) {
		return -EINVAL;
	}

	ll_timer8_drv_init(dev, config);

	return 0;
}

void ll_timer16_drv_init(TIMER16_Device *dev,
			 const struct timer_config *config)
{
	dev->TCCRnA = 0U;
	dev->TCCRnB = (config->prescaler << CSn0);
	dev->TCCRnC = 0U;

	if (config->mode == TIMER_MODE_NORMAL) {
		dev->TCNTn = config->counter;
	} else if (config->mode == TIMER_MODE_CTC) {
		/* read 16.9.2 Clear Timer on Compare Match (CTC) Mode */
		dev->TCCRnB |= BIT(WGMn2);
		dev->OCRnA = config->counter;
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