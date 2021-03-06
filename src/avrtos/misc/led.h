#ifndef _LED_H
#define _LED_H

#include <avr/io.h>

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init LED on PORTB IO 5
 */
void led_init(void);

/**
 * @brief Set LED on PORTB IO 5 to ON
 */
void led_on(void);

/**
 * @brief Set LED on PORTB IO 5 to OFF
 */
void led_off(void);

/**
 * @brief Set LED on PORTB IO 5 to state
 */
void led_set(uint8_t state);

#ifdef __cplusplus
}
#endif

#endif