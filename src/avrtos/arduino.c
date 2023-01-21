#include <avrtos/kernel.h>

#if CONFIG_ARDUINO_FRAMEWORK
void initVariant(void)
{
	z_avrtos_init();
}
#endif