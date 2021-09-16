/*___________________________________________________________________________*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

void tasks_generator(void *p);
void tasks_handler(void *p);



K_THREAD_DEFINE(generator, tasks_generator, 0x100, K_PRIO_PREEMPT(K_PRIO_HIGH), nullptr, 'G');
K_WORKQUEUE_DEFINE(workqueue, 0x200, K_PRIO_COOP(K_PRIO_LOW), 'W');

/*___________________________________________________________________________*/

struct task_t
{
  struct k_work work;
  struct k_mutex mutex;

  uint32_t input;
};

void task_handler(struct k_work* self)
{
  struct task_t * const task = CONTAINER_OF(self, struct task_t, work);

  k_sleep(K_MSEC((k_delta_ms_t) task->input));

  k_sched_lock();
  usart_hex16((uint16_t) self);
  usart_printl(" finished");
  k_sched_unlock();

  k_mutex_unlock(&task->mutex);
}

#define TASKS_COUNT   15
struct task_t tasks[TASKS_COUNT];

/*___________________________________________________________________________*/

int main(void)
{
  led_init();
  usart_init();
  
  k_thread_dump_all();

  for (uint_fast8_t i = 0; i < ARRAY_SIZE(tasks); i++)
  {
    k_mutex_init(&tasks[i].mutex);
    k_work_init(&tasks[i].work, task_handler);
    tasks[i].input = 200;
  }

  sei();

  k_sleep(K_FOREVER);
}

void tasks_generator(void *p)
{
  uint8_t i = 0;
  while(1)
  {
    if (k_mutex_lock(&tasks[i].mutex, K_NO_WAIT) == 0)
    {
      usart_hex16((uint16_t)&tasks[i]);
      usart_printl(" submitted");
      k_work_submit(&workqueue, &tasks[i].work);
    }

    i = (i + 1) % ARRAY_SIZE(tasks);
  }
}

/*___________________________________________________________________________*/