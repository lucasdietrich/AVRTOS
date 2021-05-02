#include <avr/io.h>

#include <string.h>
#include <stdio.h>

#include "uart.h"
#include "utils.h"

void function(uint16_t a, uint16_t b)
{
  usart_ram_dump(SP, 20, SP);
}