#include "led.h"

void led_set(uint8_t state)
{
        if (state) {
                led_on();
        } else {
                led_off();
        }
}