#include <avrtos/kernel.h>

#if CONFIG_ARDUINO
void initVariant(void)
{
	z_avrtos_init();
}
#endif