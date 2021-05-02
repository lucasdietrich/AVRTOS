#ifndef _LED_H
#define _LED_H

#include <avr/io.h>

/*___________________________________________________________________________*/

extern "C" void led_on(void);
extern "C" void led_off(void);
extern "C" void led_init(void);

/*___________________________________________________________________________*/

#endif