/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

void waiting_thread(void *p);
void monitoring_thread(void *p);

K_THREAD_DEFINE(waiting, waiting_thread, 0x200, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, 'W');
K_THREAD_DEFINE(monitoring, monitoring_thread, 0x200, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, 'R');

/*___________________________________________________________________________*/

K_SEM_DEFINE(mysem, 0, 1);

/*___________________________________________________________________________*/

static uint8_t usart_read_rx(void)
{
  uint8_t status = UCSR0A;
  // uint8_t resh = UCSR0B;
  uint8_t resl = UDR0;
  
  if (status & ((1 << FE0) | (1 << DOR0) | (1 << UPE0)))
  {
    usart_transmit('x');
    resl = 0;
  }
  else
  {
    // resh = (resh >> 1) & 0x01;
    // return ((resh << 8) | resl);
    usart_transmit(resl);
  }

  return resl;
}

static volatile uint8_t recv;

// atmega2560
#if defined(__AVR_ATmega328P__)
ISR(USART_RX_vect)
#elif defined(__AVR_ATmega2560__)
ISR(USART0_RX_vect)
#endif
{
  // UART0 RX buffer must be read before enabling interrupts again
  recv = usart_read_rx();

  k_sem_give(&mysem);
}

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  // set UART RX interrupt
  UCSR0B |= 1 << RXCIE0;

  sei();

  while(1)
  {
    k_sleep(K_MSEC(1000));

    usart_printl("::main");
  }
}

void waiting_thread(void *p)
{
  while (1)
  {
    if (0 == k_sem_take(&mysem, K_SECONDS(5)))
    {
      usart_print("get a semaphore, thread woke up from interrupt : ");
      usart_transmit(recv);
      usart_transmit('\n');

      k_sleep(K_MSEC(1000));
    }
    else
    {
      usart_printl("didn't get a semaphore in time");
    }
  }
}

void monitoring_thread(void *p)
{
  while (1)
  {
    k_sleep(K_MSEC(1000));

    print_runqueue();
    print_events_queue();
    dump_threads_canaries();
  }
}

/*___________________________________________________________________________*/