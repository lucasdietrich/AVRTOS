#ifndef _UART_H
#define _UART_H

#include <stddef.h>

#include <avr/io.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FOSC 16000000 // Clock Speed = F_CPU
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
#define MH16_115200_UBRR    8
#define MH16_BAUDRATE_UBRR  MH16_115200_UBRR

void usart_init();
void usart_transmit(char data);
void usart_send(const char* buffer, size_t len);
void usart_print(const char * text);
void usart_printl(const char * text);

void usart_u8(const uint8_t val);
void usart_s8(const int8_t val);
void usart_u16(uint16_t val);
void usart_hex(const uint8_t val);
void usart_hex16(const uint16_t val);
void usart_send_hex(const uint8_t* buffer, size_t len);

#ifdef __cplusplus
}
#endif

#endif