/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SERIAL_H
#define _SERIAL_H

#include <stddef.h>

#include <avrtos/kernel.h>

#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_AVRTOS_BANNER_ENABLE && !CONFIG_SERIAL_AUTO_INIT
#error "CONFIG_AVRTOS_BANNER_ENABLE requires CONFIG_SERIAL_AUTO_INIT"
#endif

/**
 * @brief Initialize the USART with custom baudrate
 *
 * @param baud Baudrate
 */
void serial_init_baud(uint32_t baud);

/**
 * @brief Initialize the USART with default baudrate
 */
__always_inline void serial_init(void)
{
    serial_init_baud(CONFIG_SERIAL_USART_BAUDRATE);
}

/**
 * @brief Print the AVRTOS banner over the USART
 */
void serial_print_banner(void);

/**
 * @brief Send a single character over the USART
 *
 * @param data
 */
void serial_transmit(char data);

/**
 * @brief Receive a single character over the USART
 *
 * @return char
 */
int serial_receive(void);

/**
 * @brief Send the buffer of known length over the USART
 *
 * @param buffer
 * @param len
 */
void serial_send(const char *buffer, size_t len);

/**
 * @brief Send the string over the USART, length is auto-calculated
 *
 * @param text
 */
void serial_print(const char *text);

/**
 * @brief Send the string over the USART with line return "\n", length is
 * auto-calculated
 *
 * @param text
 */
void serial_printl(const char *text);

/**
 * @brief Print an unsigned uint8_t in base 10 over the USART
 *
 * @param val
 */
void serial_u8(const uint8_t val);

/**
 * @brief Print a signed int8_t in base 10 over the USART
 *
 * @param val
 */
void serial_s8(const int8_t val);

/**
 * @brief Print an unsigned uint16_t in base 10 over the USART
 *
 * @param val
 */
void serial_u16(uint16_t val);

/**
 * @brief Print an unsigned uint32_t in base 10 over the USART
 * @param val
 */
void serial_u32(uint32_t val);

/**
 * @brief Print an unsigned uint8_t in base 16 over the USART
 *
 * @param val
 */
void serial_hex(const uint8_t val);

/**
 * @brief Print an unsigned uint16_t in base 16 over the USART
 *
 * @param val
 */
void serial_hex16(const uint16_t val);

/**
 * @brief Print a buffer of uint8_t in base 16 over the USART
 *
 * @param val
 */
void serial_send_hex(const uint8_t *buffer, size_t len);

/**
 * @brief Send the buffer stored in PROGMEM of known length over the USART
 *
 * @param buffer
 * @param len
 */
void serial_send_p(const char *buffer, size_t len);

/**
 * @brief Send the string stored in PROGMEM over the USART, length is
 * auto-calculated
 *
 * @param text
 */
void serial_print_p(const char *text);

/**
 * @brief Send the string stored in PROGMEM over the USART with line return
 * "\n", length is auto-calculated
 *
 * @param text
 */
void serial_printl_p(const char *text);

#ifdef __cplusplus
}
#endif

#endif /* _SERIAL_H */