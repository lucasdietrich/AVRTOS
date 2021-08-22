/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrtos/misc/uart.h"
#include "avrtos/misc/led.h"

#include "avrtos/kernel.h"
#include "avrtos/debug.h"

/*___________________________________________________________________________*/

void waiting_thread(void *p);
void monitoring_thread(void *p);

K_THREAD_DEFINE(waiting, waiting_thread, 0x200, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, nullptr, 'W');
K_THREAD_DEFINE(monitoring, monitoring_thread, 0x200, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, nullptr, 'R');

/*___________________________________________________________________________*/

K_MUTEX_DEFINE(mymutex);

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

ISR(USART_RX_vect)
{
  // UART0 RX buffer must be read before enabling interrupts again
  recv = usart_read_rx();

  k_mutex_unlock(&mymutex);
}

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  k_mutex_lock(&mymutex, K_NO_WAIT);

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
  // cli();
  while (1)
  {
    if (0 == k_mutex_lock(&mymutex, K_FOREVER))
    {
      usart_print("get the mutex, thread woke up from interrupt : ");
      usart_transmit(recv);
      usart_transmit('\n');

      k_sleep(K_MSEC(1000));

      k_mutex_lock(&mymutex, K_NO_WAIT);
    }
    else
    {
      usart_printl("didn't get the mutex");
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