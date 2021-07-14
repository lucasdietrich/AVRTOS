#ifndef _UART_H
#define _UART_H

#include <stddef.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// see datasheet
// adviced (for 16Mhz): 9600, 38400, 76800, 250k, 500k, 1M
// not recommended (for 16Mhz) (error rate) : 115200, 230400, 57.6

#define BAUD_9600       9600
#define BAUD_19200      19200
#define BAUD_14400      14400
#define BAUD_19200      19200
#define BAUD_28800      28800
#define BAUD_38400      38400
#define BAUD_57600      57600
#define BAUD_76800      76800
#define BAUD_115200     115200
#define BAUD_230400     230400 
#define BAUD_250000     250000
#define BAUD_500000     500000
#define BAUD_1000000    1000000
#define BAUDRATE BAUD_500000

#define UBRR ((F_CPU >> 4) / BAUDRATE) - 1

/**
 * @brief Initiliaze the USART with the baudrate specified as argument
 * 
 * @param baudrate_ubrr 
 */
void _usart_init(const uint16_t baudrate_ubrr);

/**
 * @brief Initialize the USART at 115200 bauds/s
 */
void usart_init();

/**
 * @brief Send a single character over the USART
 * 
 * @param data 
 */
void usart_transmit(char data);

/**
 * @brief Send the buffer of known length over the USART
 * 
 * @param buffer 
 * @param len 
 */
void usart_send(const char* buffer, size_t len);

/**
 * @brief Send the string over the USART, length is auto-calculated
 * 
 * @param text 
 */
void usart_print(const char * text);

/**
 * @brief Send the string over the USART with line return "\n", length is auto-calculated
 * 
 * @param text 
 */
void usart_printl(const char * text);

/**
 * @brief Print an unsigned uint8_t in base 10 over the USART
 * 
 * @param val 
 */
void usart_u8(const uint8_t val);

/**
 * @brief Print a signed int8_t in base 10 over the USART
 * 
 * @param val 
 */
void usart_s8(const int8_t val);

/**
 * @brief Print an unsigned uint16_t in base 10 over the USART
 * 
 * @param val 
 */
void usart_u16(uint16_t val);

/**
 * @brief Print an unsigned uint8_t in base 16 over the USART
 * 
 * @param val 
 */
void usart_hex(const uint8_t val);

/**
 * @brief Print an unsigned uint16_t in base 16 over the USART
 * 
 * @param val 
 */
void usart_hex16(const uint16_t val);

/**
 * @brief Print a buffer of uint8_t in base 16 over the USART
 * 
 * @param val 
 */
void usart_send_hex(const uint8_t* buffer, size_t len);


/**
 * @brief Send the buffer stored in PROGMEM of known length over the USART
 * 
 * @param buffer 
 * @param len 
 */
void usart_send_p(const char* buffer, size_t len);

/**
 * @brief Send the string stored in PROGMEM over the USART, length is auto-calculated
 * 
 * @param text 
 */
void usart_print_p(const char * text);

/**
 * @brief Send the string stored in PROGMEM over the USART with line return "\n", length is auto-calculated
 * 
 * @param text 
 */
void usart_printl_p(const char * text);

#ifdef __cplusplus
}
#endif

#endif