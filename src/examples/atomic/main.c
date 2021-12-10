#include <avrtos/misc/uart.h>

#include <avrtos/kernel.h>

#define K_MODULE K_MODULE_APPLICATION

K_ATOMIC_DEFINE(a1, 0xFF);

int main(void)
{
        usart_init();

        sei();

        atomic_val_t val;

        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_clear(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Inc");

        val = atomic_inc(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_inc(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Dec");

        val = atomic_dec(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_dec(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Or");

        val = atomic_or(&a1, 0xA0);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_or(&a1, 0xAA);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("And");

        val = atomic_and(&a1, 0x0F);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_and(&a1, 0x07);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Xor");

        val = atomic_xor(&a1, 0xF0);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_xor(&a1, 0x0F);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Test");

        val = atomic_test_bit(&a1, 7);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Test & clear");

        val = atomic_test_and_clear_bit(&a1, 7);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Test & clear");

        val = atomic_test_and_clear_bit(&a1, 7);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Test & set");

        val = atomic_test_and_set_bit(&a1, 7);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_test_and_set_bit(&a1, 7);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Clear");

        atomic_clear(&a1);

        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        atomic_clear(&a1);

        usart_printl("Set bit");

        atomic_set_bit(&a1, 5);
        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Clear bit");

        atomic_clear_bit(&a1, 5);
        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Set bit True");

        atomic_set_bit_to(&a1, 5, true);
        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Blind clear");

        atomic_set_bit_to(&a1, 5, false);
        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();

        usart_printl("Set bit False");

        atomic_blind_clear(&a1);
        val = atomic_get(&a1);
        usart_hex(val);
        usart_transmit('\n');

        __ASSERT_INTERRUPT();
}
