#ifndef _AVRTOS_PRDM_H
#define _AVRTOS_PRDM_H

/* 
 * Pseudo random number generator (based on LFSR method)
 * Sources :
 * - https://www.microchip.com/forums/tm.aspx?m=1117824&mpage=1
 * - https://www.maximintegrated.com/en/design/technical-documents/app-notes/4/4400.html
 */

#include <avr/io.h>
#include <stdint.h>
#include <avrtos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct k_prng
{
        uint32_t lfsr32;
        uint32_t lfsr31;
};

#define K_PRNG_POLY_MASK_32     0xB4BCD35C
#define K_PRNG_POLY_MASK_31     0x7A5BC2E3

/* default seed values */
#define K_PRNG_DEFAULT_LFSR32   0xABCDE
#define K_PRNG_DEFAULT_LFSR31   0x23456789

#define K_PRNG_INITIALIZER(lfsr32_val, lfsr31_val)      \
{                                                       \
        .lfsr32 = lfsr32_val,                           \
        .lfsr31 = lfsr31_val                            \
}   

#define K_PRNG_DEFINE(prng_name, lfsr32_val, lfsr31_val)    \
    struct k_prng prng_name = K_PRNG_INITIALIZER(lfsr32_val, lfsr31_val)

#define K_PRNG_DEFINE_DEFAULT(prng_name) \
    K_PRNG_DEFINE(prng_name, K_PRNG_DEFAULT_LFSR32, K_PRNG_DEFAULT_LFSR31)

/*___________________________________________________________________________*/

K_NOINLINE uint16_t k_prng_get(struct k_prng* prng);

K_NOINLINE uint32_t k_prng_get_u32(struct k_prng* prng);

K_NOINLINE void k_prng_get_buffer(struct k_prng *prng,
        uint8_t *buffer, uint16_t len);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif