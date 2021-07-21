#ifndef _AVRTOS_DEFINESC_H
#define _AVRTOS_DEFINESC_H

#if !__ASSEMBLER__

#include <stdint.h>
#include <stddef.h>

#if KERNEL_HIGH_RANGE_TIME_OBJECT_U32 == 1
typedef uint32_t k_delta_ms_t;
#else
typedef uint16_t k_delta_ms_t;
#endif

typedef struct
{
    k_delta_ms_t value;
} k_timeout_t;

#define K_MSEC(delay_ms)       ((k_timeout_t){delay_ms})
#define K_NO_WAIT()    ((k_timeout_t){0})
#define K_FOREVER()    ((k_timeout_t){-1})

#define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))

#endif
#endif