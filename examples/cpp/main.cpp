/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include <avrtos/avrtos.h>
#include <avrtos/debug.h>

K_MEM_SLAB_DEFINE(stacks, 0x80, 4U);

class Task;

typedef void (*task_func_t)(Task &task);

class Task
{
  public:
	Task(const char name[],
		 task_func_t entry,
		 uint8_t priority = Z_THREAD_PRIO_COOP | Z_THREAD_PRIO_LOW)
	{

		void *stack;
		k_mem_slab_alloc(&stacks, &stack, K_NO_WAIT);
		k_thread_create(&_ctx, _wrap_entry, stack, 0x80, priority, this, ++cnt + '0');

		memcpy(_name, name, sizeof(_name));
		_entry = entry;
	}

	void start(void)
	{
		k_thread_start(&_ctx);
	}
	void sleep(k_timeout_t timeout)
	{
		k_sleep(timeout);
	}
	void print_name(void)
	{
		serial_print(_name);
		serial_transmit('\n');
	}

  private:
	static uint8_t cnt;
	static void _wrap_entry(void *arg)
	{
		Task *task = static_cast<Task *>(arg);
		task->_entry(*task);
	}

	task_func_t _entry;
	char _name[0x10];
	struct k_thread _ctx;
};

uint8_t Task::cnt = 0u;

void func(Task &task)
{
	for (;;) {
		task.print_name();
		task.sleep(K_MSEC(1000));
	}
}

int main()
{
	serial_init();
	k_thread_dump_all();

	Task task1 = Task("task1", func);
	Task task2 = Task("task2", func);

	task1.start();
	task2.start();

	k_sleep(K_FOREVER);
}