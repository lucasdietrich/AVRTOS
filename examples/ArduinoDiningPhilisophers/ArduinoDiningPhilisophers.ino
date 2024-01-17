/*
 * Copyright (c) 2023 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos.h>

#include <avrtos/logging.h>
// Change the log level (LOG_LEVEL_DBG, LOG_LEVEL_INF, LOG_LEVEL_WRN or LOG_LEVEL_ERR)
#define LOG_LEVEL LOG_LEVEL_WRN

#define MUTEX	  0
#define SEMAPHORE 1
#define FIFO	  2

// Cchange the fork implementation (MUTEX, SEMAPHORE or FIFO)
#define FORKS MUTEX

#define PHIL_THINKING_DURATION_MAX_MS 5000u
#define PHIL_EATING_DURATION_MAX_MS   5000u
#define PHIL_DURATION_TO_STARVE_MS    15000u

typedef enum {
	PHIL_STATE_NONE = 0u,
	PHIL_STATE_THINKING,
	PHIL_STATE_PENDING_LEFT_FORK,
	PHIL_STATE_PENDING_RIGHT_FORK,
	PHIL_STATE_EATING,
	PHIL_STATE_STARVING,
} phil_state_t;

char phil_state_to_char(phil_state_t state)
{
	static char states_char[] = {
		[PHIL_STATE_NONE]		= '-',
		[PHIL_STATE_THINKING]		= 'K',
		[PHIL_STATE_PENDING_LEFT_FORK]	= '1',
		[PHIL_STATE_PENDING_RIGHT_FORK] = '2',
		[PHIL_STATE_EATING]		= 'E',
		[PHIL_STATE_STARVING]		= 'S',
	};

	if (state >= ARRAY_SIZE(states_char)) {
		return '?';
	}

	return states_char[state];
}

static uint16_t get_random_duration_ms(uint16_t min_duration_ms, uint16_t max_duration_ms)
{
	static K_PRNG_DEFINE_DEFAULT(prng);
	uint16_t rdm;

	/* Lock the scheduler to prevent preemption (in case of K_PREEPTIVE priority),
	 * "prng" structure is shared between threads.
	 */
	k_sched_lock();
	rdm = k_prng_get(&prng);
	k_sched_unlock();

	rdm = min_duration_ms + (rdm % (max_duration_ms - min_duration_ms));

	return rdm;
}

class Fork
{
public:
	Fork() = default;

	virtual bool Lock(k_timeout_t timeout) = 0;
	virtual void Unlock(void)	       = 0;
};

#if FORKS == MUTEX

#define FORK MutexFork

class MutexFork : public Fork
{
public:
	MutexFork(void)
	{
		/* Create a mutex for the fork */
		k_mutex_init(&m_mutex);
	}

	bool Lock(k_timeout_t timeout) override
	{
		return k_mutex_lock(&m_mutex, timeout) == 0;
	}

	void Unlock(void) override
	{
		k_mutex_unlock(&m_mutex);
	}

private:
	/* Mutex for the fork */
	struct k_mutex m_mutex;
};

#elif FORKS == SEMAPHORE

#define FORK SemaphoreFork

class SemaphoreFork : public Fork
{
public:
	SemaphoreFork(void)
	{
		/* Create a semaphore for the fork */
		k_sem_init(&m_sem, 1u, 1u);
	}

	bool Lock(k_timeout_t timeout) override
	{
		return k_sem_take(&m_sem, timeout) == 0;
	}

	void Unlock(void) override
	{
		k_sem_give(&m_sem);
	}

private:
	/* Mutex for the fork */
	struct k_sem m_sem;
};

#elif FORKS == FIFO

#define FORK FifoFork

class FifoFork : public Fork
{
public:
	FifoFork(void)
	{
		/* Create a fifo for the fork */
		k_fifo_init(&m_fifo);

		/* Put the node in the fifo */
		this->Unlock();
	}

	bool Lock(k_timeout_t timeout) override
	{
		return k_fifo_get(&m_fifo, timeout) != nullptr;
	}

	void Unlock(void) override
	{
		k_fifo_put(&m_fifo, &m_node);
	}

private:
	/* Mutex for the fork */
	struct k_fifo m_fifo;
	struct snode m_node;
};

#endif

class Philosopher
{
public:
	Philosopher(Fork &left, Fork &right, const char *name)
		: m_left_fork(left), m_right_fork(right), m_name(name)
	{
		k_thread_create(&m_thread,
				reinterpret_cast<k_thread_entry_t>(Philosopher::Live),
				m_stack,
				sizeof(m_stack),
				K_COOPERATIVE,
				this,
				m_name[0u]);

		m_state = PHIL_STATE_NONE;

		m_philosophers_count++;
	}

	void LunchTime(void)
	{
		k_thread_start(&m_thread);
		LOG_INF("%s lunch time !", m_name);
	}

	void Debug(void)
	{
		k_print_stack_canaries(&m_thread);
	}

	phil_state_t GetState(void)
	{
		return m_state;
	}

private:
	void Think(void)
	{
		m_state = PHIL_STATE_THINKING;
		const uint16_t duration =
			get_random_duration_ms(0u, PHIL_THINKING_DURATION_MAX_MS);
		LOG_INF("%s is thinking for %u ms", m_name, duration);
		k_sleep(K_MSEC(duration));
		LOG_INF("%s is hungry", m_name);
	}

	bool GetForks(void)
	{
		bool has_left_fork  = false;
		bool has_right_fork = false;
		uint16_t time_left  = PHIL_DURATION_TO_STARVE_MS;
		uint32_t now	    = k_uptime_get_ms32();

		LOG_INF("%s is trying to get forks", m_name);

		m_state	      = PHIL_STATE_PENDING_LEFT_FORK;
		has_left_fork = m_left_fork.Lock(K_MSEC(time_left));

		if (has_left_fork) {
			time_left -= k_uptime_get_ms32() - now;
		} else {
			LOG_WRN("%s failed to get left fork", m_name);
			goto exit;
		}

		m_state	       = PHIL_STATE_PENDING_RIGHT_FORK;
		has_right_fork = m_right_fork.Lock(K_MSEC(time_left));

		if (has_right_fork) {
			LOG_INF("%s got forks", m_name);
		} else {
			LOG_WRN("%s failed to get right fork", m_name);
			m_left_fork.Unlock();
			goto exit;
		}

	exit:
		return has_left_fork && has_right_fork;
	}

	void Eat(void)
	{
		m_state = PHIL_STATE_EATING;
		const uint16_t duration =
			get_random_duration_ms(0u, PHIL_EATING_DURATION_MAX_MS);
		LOG_INF("%s is eating for %u ms", m_name, duration);
		k_sleep(K_MSEC(duration));

		LOG_INF("%s is done eating", m_name);

		m_left_fork.Unlock();
		m_right_fork.Unlock();
	}

	void Starve(void)
	{
		m_state = PHIL_STATE_STARVING;
		LOG_ERR("%s starved to death", m_name);

		/* Stop the thread */
		k_stop();
	}

	static void Live(Philosopher *phil)
	{
		for (;;) {
			phil->Think();

			const bool got_forks = phil->GetForks();

			if (got_forks) {
				phil->Eat();
			} else {
				phil->Starve();
			}
		}
	}

	constexpr static uint16_t k_stack_size =
		160; // why name is truncated with stack size < 128?

	static uint8_t m_philosophers_count;

	Fork &m_left_fork;
	Fork &m_right_fork;

	const char *m_name;

	struct k_thread m_thread;
	uint8_t m_stack[k_stack_size];

	phil_state_t m_state;
};

uint8_t Philosopher::m_philosophers_count = 0u;

static FORK forks[5u];

/* Dijkstra's solution, always pick the lowest numbered fork first */
static Philosopher philosophers[5u] = {
	Philosopher(forks[0u], forks[1u], "Aristotle"),
	Philosopher(forks[1u], forks[2u], "Kant"),
	Philosopher(forks[2u], forks[3u], "Buddha"),
	Philosopher(forks[3u], forks[4u], "Marx"),
	Philosopher(forks[0u], forks[4u], "Russel"),
};

struct k_timer timer;
struct k_sem sem;

void timer_handler(struct k_timer *timer)
{
	k_sem_give(&sem);
}


void setup(void)
{
	/* LED initialisation */
	pinMode(LED_BUILTIN, OUTPUT);

	/* Serial initialisation */
	Serial.begin(9600u);

	Serial.println("\n\nPhilosophers example started: ");
	Serial.println("\t - Not started");
	Serial.println("\t T Thinking");
	Serial.println("\t 1 Pending left fork");
	Serial.println("\t 2 Pending right fork");
	Serial.println("\t E Eating");
	Serial.println("\t S Starving");
	Serial.println("==================================");
	Serial.flush();

	for (Philosopher &phil : philosophers) {
		phil.LunchTime();
	}

	k_sem_init(&sem, 1u, 1u);
	k_timer_init(&timer, timer_handler, K_MSEC(100u), K_NO_WAIT);
}

void loop(void)
{
	k_sem_take(&sem, K_FOREVER);

	k_show_uptime();

	for (Philosopher &phil : philosophers) {
		phil_state_t state = phil.GetState();

		printf_P(PSTR("%c "), phil_state_to_char(state));
	}

	printf_P(PSTR("\n"));
}