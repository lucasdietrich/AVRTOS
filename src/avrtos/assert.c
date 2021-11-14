#include "assert.h"

#include <avr/pgmspace.h>

#include "misc/uart.h"

static const char *const module_to_str(uint8_t module)
{
        /* TODO optimize size */
        static const char modules_str[][sizeof("APPLICATION")] PROGMEM = {
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
            {"MEMSLAB"},
            {"TIMER"},
            {"MSGQ"},
            {""},
            {""},
            {""},
            {""},
            {""},
            {""},
            {""},
            {""},
            {""},
            {""},
            {""},
            {""},
            {""},
            {""},
            {"APPLICATION"}
        };

        return modules_str[MIN(module, ARRAY_SIZE(modules_str))];
}

static const char *const acode_to_str(uint8_t acode)
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
            {K_ASSERT_TRUE, "TRUE"},
            {K_ASSERT_FALSE, "FALSE"},
            {K_ASSERT_WORKQUEUE, "WORKQUEUE"},
            {K_ASSSERT_NOTNULL, "NOTNULL"},
            {K_ASSSERT_NULL, "NULL"}
        };

        for (uint_fast8_t i = 1; i < ARRAY_SIZE(acodes_str); i++) {
                if ((uint8_t)pgm_read_byte(&acodes_str[i].acode) == acode) {
                        return acodes_str[i].name;
                }
        }
        return acodes_str[0].name;
}

void __assert(uint8_t expression, uint8_t module, uint8_t acode, uint16_t line)
{
        static const char assert_msg[] PROGMEM =
                "***** Kernel Assertion failed *****\n  K_MODULE_";
        static const char assert_acode_msg[] PROGMEM = " L [K_ASSERT_";

        if (expression == 0) {
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