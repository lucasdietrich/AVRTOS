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
#define K_NO_WAIT    ((k_timeout_t){0})
#define K_FOREVER    ((k_timeout_t){(k_delta_ms_t) -1})

#define K_SWAP_ENDIANNESS(n) (((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8))

#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define CONTAINER_OF(ptr, type, field) ((type *)(((char *)(ptr)) - offsetof(type, field)))

#define SET_BIT(x, b)  ((x) |= b)
#define CLR_BIT(x, b)  ((x) &= (~(b)))


#endif
#endif