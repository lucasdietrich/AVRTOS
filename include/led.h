#ifndef _LED_H
#define _LED_H

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

void led_on(void);
void led_off(void);
void led_init(void);

#ifdef __cplusplus
}
#endif

#endif