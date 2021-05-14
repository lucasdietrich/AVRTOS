#ifndef _UART_H
#define _UART_H

#include <stddef.h>

#include <avr/io.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Following constants are calculated for a CPU Clock running at 16MHz
#ifdef F_CPU
#define FOSC F_CPU
#else
#define FOSC 16000000 
#endif

#define BAUD_9600       9600
#define BAUD_19200      19200
#define BAUD_38400      38400
#define BAUD_57600      57600
#define BAUD_115200     115200

// FOSC/16/BAUD - 1 = 103.166 -> 103
#define MH16_9600_UBRR      103
#define MH16_19200_UBRR     51
#define MH16_38400_UBRR     25
#define MH16_57600_UBRR     16
#define MH16_115200_UBRR    8   // particular case 

#define MH16_DEFAULT_BAUDRATE_UBRR  MH16_115200_UBRR

/**
 * @brief Initiliaze the USART with the baudrate specified as argument
 * 
 * @param baudrate_ubrr 
 */
void _usart_init(const uint8_t baudrate_ubrr);

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

#ifdef __cplusplus
}
#endif

#endif