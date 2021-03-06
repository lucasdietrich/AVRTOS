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

	usart_printl_p(PSTR("Inc"));

	val = atomic_inc(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	val = atomic_inc(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl_p(PSTR("Dec"));

	val = atomic_dec(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	val = atomic_dec(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl_p(PSTR("Or"));

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

	usart_printl_p(PSTR("And"));

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

	usart_printl_p(PSTR("Xor"));

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

	usart_printl_p(PSTR("Test"));

	val = atomic_test_bit(&a1, 7);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl_p(PSTR("Test & clear"));

	val = atomic_test_and_clear_bit(&a1, 7);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl_p(PSTR("Test & clear"));

	val = atomic_test_and_clear_bit(&a1, 7);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl_p(PSTR("Test & set"));

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

	usart_printl_p(PSTR("Clear"));

	atomic_clear(&a1);

	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	atomic_clear(&a1);

	usart_printl_p(PSTR("Set bit"));

	atomic_set_bit(&a1, 5);
	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl_p(PSTR("Clear bit"));

	atomic_clear_bit(&a1, 5);
	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl_p(PSTR("Set bit True"));

	atomic_set_bit_to(&a1, 5, true);
	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl_p(PSTR("Blind clear"));

	atomic_set_bit_to(&a1, 5, false);
	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl_p(PSTR("Set bit False"));

	atomic_blind_clear(&a1);
	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	usart_printl("Cas");

	val = atomic_cas(&a1, 0x00, 0xAA);
	usart_hex(val);
	usart_transmit(':');
	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	val = atomic_cas(&a1, 0xAA, 0xBB);
	usart_hex(val);
	usart_transmit(':');
	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();

	val = atomic_cas(&a1, 0xAA, 0xCC);
	usart_hex(val);
	usart_transmit(':');
	val = atomic_get(&a1);
	usart_hex(val);
	usart_transmit('\n');

	__ASSERT_INTERRUPT();
}
