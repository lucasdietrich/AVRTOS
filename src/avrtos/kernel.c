#include "kernel.h"

/*___________________________________________________________________________*/

static struct ditem *runqueue = &k_thread_main.tie.runqueue; 

static struct titem *events_queue = NULL;

void k_queue(struct thread_t *th)
{
    push_back(runqueue, &th->tie.runqueue);
}

void k_yield(void)
{
    _k_yield();
}

extern struct thread_t __k_threads_start;
extern struct thread_t __k_threads_end;
extern struct mutex_t __k_mutexes_start;
extern struct mutex_t __k_mutexes_end;

void _k_kernel_init(void)
{
    k_thread.count = &__k_threads_end - &__k_threads_start;

    // main thread is the first running (ready or not)
    for (uint8_t i = 0; i < k_thread.count; i++)
    {
        if (k_thread.list[i].state == READY) // only queue ready threads
        {
            dlist_queue(runqueue, &k_thread.list[i].tie.runqueue);
        }
    }
}

#include "misc/uart.h"

#if KERNEL_SCHEDULER_DEBUG

// todo write this function in assembly
// ! mean tqueue item
// ~ thread set to waiting
// > default next thread
struct thread_t *_k_scheduler(void)
{
    // print_tqueue(events_queue, _thread_symbol);
    void *next = (struct titem*) tqueue_pop(&events_queue);
    if (next != NULL)
    {
        push_ref(&runqueue, next);

        usart_transmit('!');
    }
    else if (k_thread.current->state == WAITING)
    {
        usart_transmit('~');
    }
    else
    {
 
        // next = (struct ditem *)ref_requeue_top(&runqueue);
        ref_requeue_top(&runqueue);

        usart_transmit('>');
    }

    _thread_symbol(runqueue);

    THREAD_OF_TITEM(runqueue)->state = READY;

    return k_thread.current = CONTAINER_OF(runqueue, struct thread_t, tie);
}
#else
struct thread_t *_k_scheduler(void)
{
    // print_tqueue(events_queue, _thread_symbol);
    void *next = (struct titem*) tqueue_pop(&events_queue);
    if (next != NULL)
    {
        push_ref(&runqueue, next);
    }
    else if(k_thread.current->state != WAITING)
    {
        ref_requeue_top(&runqueue);
    }
    THREAD_OF_TITEM(runqueue)->state = READY;

    return k_thread.current = CONTAINER_OF(runqueue, struct thread_t, tie);
}
#endif

#include "avrtos/dstruct/debug.h"

void _thread_symbol(struct ditem * item)
{
    usart_transmit(CONTAINER_OF(item, struct thread_t, tie.runqueue)->symbol);
}

void _thread_symbol_e(struct titem * item)
{
    usart_transmit(CONTAINER_OF(item, struct thread_t, tie.event)->symbol);
}

void print_runqueue(void)
{
    print_dlist(runqueue, _thread_symbol);
}

void print_events_queue(void)
{
    print_tqueue(events_queue, _thread_symbol_e);
}

void k_cpu_idle(void)
{
    // TODO loop on low power and regularly check scheduler
}

/*___________________________________________________________________________*/

#include "misc/uart.h"

void _k_system_shift(void)
{
    tqueue_shift(&events_queue, KERNEL_TIME_SLICE);
}

// todo k_sleep(FOREVER) -> remov thread from queue only
void k_sleep(k_timeout_t timeout)
{
    if (timeout.value != K_NO_WAIT.value)
    {
        cli();

        k_thread.current->state = WAITING;
        
        pop_ref(&runqueue);

        if(timeout.value != K_FOREVER.value)
        {
            tqueue_schedule(&events_queue, &k_thread.current->tie.event, timeout.value);
        }
     
        k_yield();

        sei();
    }
}

/*___________________________________________________________________________*/

uint8_t mutex_lock_wait(mutex_t *mutex, k_timeout_t timeout)
{
    uint8_t lock = _mutex_lock(mutex);
    if ((lock != 0) && (timeout.value != K_NO_WAIT.value))
    {
        cli();

        k_thread.current->state = WAITING;

        pop_ref(&runqueue);

        queue(&mutex->waitqueue, &k_thread.current->wmutex);

        if(timeout.value != K_FOREVER.value)
        {
            tqueue_schedule(&events_queue, &k_thread.current->tie.event, timeout.value);
        }

        k_yield();

#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('{');
        _thread_symbol(runqueue);
#endif
        lock = _mutex_lock(mutex);
        sei();
    }
#if KERNEL_SCHEDULER_DEBUG
    usart_transmit('#');
#endif
    return lock;
}

void mutex_release(mutex_t *mutex)
{
    cli();
    struct qitem* first_waiting_thread = dequeue(&mutex->waitqueue);
    if (first_waiting_thread == NULL)
    {
        _mutex_release(mutex);
    }
    if (first_waiting_thread != NULL)
    {
        struct thread_t *th = THREAD_OF_QITEM(first_waiting_thread);
        
        // remove from queue
        tqueue_remove(&events_queue, &th->tie.event);

        th->state = READY;

        th->wmutex.next = NULL;

        push_front(runqueue, &th->tie.runqueue);
        
#if KERNEL_SCHEDULER_DEBUG
        usart_transmit('{');
        _thread_symbol(runqueue);
        usart_transmit('*');
#endif
        _mutex_release(mutex);

        k_yield();
    }
    sei();
}