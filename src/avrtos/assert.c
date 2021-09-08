#include "assert.h"

#include <avr/pgmspace.h>

#include "misc/uart.h"

#define K_MODULE_AVRTOS         0
#define K_MODULE_KERNEL         1
#define K_MODULE_ARCH           2
#define K_MODULE_SYSCLOCK       3
#define K_MODULE_THREAD         4
#define K_MODULE_IDLE           5

#define K_MODULE_MUTEX          10
#define K_MODULE_SEMAPHORE      11
#define K_MODULE_SIGNAL         12
#define K_MODULE_WORKQUEUE      13
#define K_MODULE_FIFO           14
#define K_MODULE_MEMSLAB        15

static const char * const module_to_str(uint8_t module)
{
    static const char modules_str[][sizeof("WORKQUEUE")] PROGMEM = {
        {"AVRTOS"},
        {"KERNEL"},
        {"ARCH"},
        {"SYSCLOCK"},
        {"THREAD"},
        {"IDLE"},
        {""},
        {""},
        {""},
        {""},
        {"MUTEX"},
        {"SEMAPHORE"},
        {"SIGNAL"},
        {"WORKQUEUE"},
        {"FIFO"},
        {"MEMSLAB"}
    };

    return modules_str[MIN(module, ARRAY_SIZE(modules_str))];
}

static const char * const acode_to_str(uint8_t acode)
{
    static const struct {
        uint8_t acode;
        char name[sizeof("LEASTONE_RUNNING")];
    } acodes_str[] PROGMEM = {
        {K_ASSERT_UNDEFINED, "UNDEFINED"},
        {K_ASSERT_INTERRUPT, "INTERRUPT"},
        {K_ASSERT_NOINTERRUPT, "NOINTERRUPT"},
        {K_ASSERT_LEASTONE_RUNNING, "LEASTONE_RUNNING"},
        {K_ASSERT_THREAD_STATE, "THREAD_STATE"},
        {K_ASSERT_WORKQUEUE, "WORKQUEUE"},
        {K_ASSSERT_NOTNULL, "NOTNULL"},
        {K_ASSSERT_NULL, "NULL"}
    };

    for (uint_fast8_t i = 1; i < ARRAY_SIZE(acodes_str); i++)
    {
        if ((uint8_t) pgm_read_byte(&acodes_str[i].acode) == acode)
        {
            return acodes_str[i].name;
        }
    }
    return acodes_str[0].name;
}

void __assert(uint8_t expression, uint8_t module, uint8_t acode, uint16_t line)
{
    static const char assert_msg[] PROGMEM = "***** Kernel Assertion failed *****\n  K_MODULE_";
    static const char assert_acode_msg[] PROGMEM = " L [K_ASSERT_";

    if (expression == 0)
    {
        cli();
        
        usart_print_p(assert_msg);

        usart_print_p(module_to_str(module));
        usart_transmit(':');
        usart_u16(line);
        usart_print_p(assert_acode_msg);
        usart_print_p(acode_to_str(acode));
        usart_transmit(']');

        asm("jmp _exit");

        __builtin_unreachable();
    }
}