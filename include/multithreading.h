#ifndef _MULTITHREADING_H
#define _MULTITHREADING_H

#include <avr/io.h>

#include <stddef.h>

#include <avr/pgmspace.h>

// todo remove
#include <uart.h>
#include <multithreading_debug.h>

/*___________________________________________________________________________*/

#define CONFIG_THREAD_MAX  4
#define CONFIG_THREAD_MAIN_STACK_SIZE       0x200
#define CONFIG_THREAD_MAIN_THREAD_PRIORITY  K_PRIO_PREEMPT(8)

/*___________________________________________________________________________*/

#define K_SWAP_LITTLE_BIG_ENDIAN(u16) (((uint16_t)u16 << 8) | ((uint16_t)u16 >> 8))

#define K_THREAD_DEFAULT_SREG 0x02 // interrupt flag set only

#define K_THREAD_STACK_VOID_SIZE 35u
#define K_THREAD_STACK_MIN_SIZE K_THREAD_STACK_VOID_SIZE

#define K_PRIO_PREEMPT(p) (p)
#define K_PRIO_COOP(p) (-p)
#define K_PRIO_DEFAULT K_PRIO_COOP(1)
#define K_STOPPED 0

#define K_STACK_END(stack_start, size) ((void *)((uint16_t)stack_start + size - 1u))
#define K_STACK_START(stack_end, size) ((void *)((uint16_t)stack_end - size + 1u))

#define K_STACK_INIT_SP(stack_end) ((uint16_t)stack_end - K_THREAD_STACK_VOID_SIZE)

#define K_THREAD_STACK_NAME(name) (&_k_stack_buf_##name)

#define K_STACK_INIT_SP_FROM_NAME(name, stack_size) K_STACK_INIT_SP(K_STACK_END(K_THREAD_STACK_NAME(name), stack_size))

#define K_THREAD_STACK_DEFINE(name, size) char _k_stack_buf_##name[size]

#define K_THREAD_STACK_SIZE(name) (sizeof(_k_stack_buf_##name))

#define _K_STACK_INITIALIZER(name, stack_size, entry, context_p) \
    struct                                                       \
    {                                                            \
        uint8_t empty[stack_size - K_THREAD_STACK_VOID_SIZE];    \
        struct                                                   \
        {                                                        \
            uint8_t sreg;                                        \
            uint8_t r0r23[6u];                                   \
            void *context;                                       \
            uint8_t r26r31[24u];                                 \
            void *ret_addr;                                      \
        } base;                                                  \
    } _k_stack_buf_##name = {                                    \
        {0x00},                                                  \
        {K_THREAD_DEFAULT_SREG,                                  \
         {0x00},                                                 \
         (void *)K_SWAP_LITTLE_BIG_ENDIAN((uint16_t)context_p),  \
         {0x00},                                                 \
         (void *)K_SWAP_LITTLE_BIG_ENDIAN((uint16_t)entry)}}

// in order to remove the K_SWAP_LITTLE_BIG_ENDIAN define here we need to pop and push registers in an inverted way

#define _K_THREAD_INITIALIZER(name, entry_p, stack_size, prio, local_storage_p)                   \
    {                                                                                             \
        .sp = (void *)K_STACK_INIT_SP_FROM_NAME(name, stack_size),                                \
        .priority = prio,                                                                         \
        .stack = {.end = K_STACK_END(K_THREAD_STACK_NAME(name), stack_size), .size = stack_size}, \
        .entry = (thread_entry_t)entry_p,                                                         \
        .local_storage = (void *)local_storage_p                                                  \
    }

#define K_THREAD_DEFINE(name, entry, stack_size, prio, context_p, local_storage_p)         \
    __attribute__((used)) static _K_STACK_INITIALIZER(name, stack_size, entry, context_p); \
    __attribute__((used)) static thread_t name = _K_THREAD_INITIALIZER(name, entry, stack_size, prio, local_storage_p);

// todo remove
#define THREAD_2ND_STACK_LOC RAMSTART + (RAMEND - RAMSTART) / 2

/*___________________________________________________________________________*/

#define THREAD_USE_INIT_STACK_ASM       1

/*___________________________________________________________________________*/

#ifdef CONFIG_THREAD_MAX
#if CONFIG_THREAD_MAX > 1
#define K_THREAD_MAX_COUNT CONFIG_THREAD_MAX
#else
#error Cannot configure this library for less than 2 threads (CONFIG_THREAD_MAX < 2)
#endif
#else
#define K_THREAD_MAX_COUNT 4
#endif

#ifdef CONFIG_THREAD_MAIN_STACK_SIZE
#define THREAD_MAIN_STACK_SIZE  CONFIG_THREAD_MAIN_STACK_SIZE
#else
#define THREAD_MAIN_STACK_SIZE  RAMEND - RAMSTART
#endif 

#ifdef CONFIG_THREAD_MAIN_THREAD_PRIORITY
#define THREAD_MAIN_THREAD_PRIORITY   CONFIG_THREAD_MAIN_THREAD_PRIORITY
#else
#define THREAD_MAIN_THREAD_PRIORITY   K_PRIO_PREEMPT(8)
#endif
/*___________________________________________________________________________*/

typedef void (*thread_entry_t)(void*);

// Size : 11B
// "thread_t" must be static !!
struct thread_t
{
    void* sp;       // stack pointer (16 bits)
    int8_t priority; // cooperative < 0, preemptive > 0, 0 : off
    struct {
        void * end;
        size_t size;
    } stack;
    thread_entry_t entry;
    void * local_storage;
};

// Size : 3B + 2B*THREAD_MAX
// duplication : current and current_idx does represent the same thing
// todo remove current and shift in multithreading.asm
struct k_thread_meta {
    thread_t *current;                            // used in multithreading.asm
    struct thread_t * list[K_THREAD_MAX_COUNT];   // main thread is 0
    uint8_t count;
    uint8_t current_idx;                          // current index
};

extern struct thread_t k_thread_main;

extern struct k_thread_meta k_thread;

/*___________________________________________________________________________*/

/**
 * @brief Define a second thread and prepare stack
 * 
 * @param th thread structure pointer
 * @param entry thread function to call
 * @param stack thread stack start location
 * @param p thread context
 */

#if THREAD_USE_INIT_STACK_ASM
extern "C" void k_thread_stack_create(struct thread_t *const th, thread_entry_t entry, void *const stack, void *const context_p);
#else
void k_thread_stack_create(struct thread_t *const th, thread_entry_t entry, void *const stack_end, void *const context_p);
#endif

void k_thread_create(struct thread_t *const th, thread_entry_t entry, void *const stack, const size_t stack_size, const int8_t priority, void *const context_p, void *const local_storage);

int k_thread_register(struct thread_t *const th);

/**
 * @brief TODO
 * 
 */
extern "C" void k_thread_switch(struct thread_t *from, struct thread_t *to);

// disable name mangling
extern "C" struct thread_t *k_schedule(void);


// TODO

extern "C" void k_yield(void);

void cpu_idle(void);

/*___________________________________________________________________________*/

#endif