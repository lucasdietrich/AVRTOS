#include <stdint.h>
#include <stddef.h>

#include "avrtos/sys.h"
#include "sd.h"
#include "sd_priv.h"

// G(x) = x^7 + x^3 + 1 (0b10001001)
#define CRC7_POLY 0x89u

uint8_t crc7(uint8_t *data, size_t len)
{
    uint8_t crc = 0u;

    while(len-- > 0u) {
        crc ^= *data++;
        for (uint8_t i = 0u; i < 8u; i++) {
            if (crc & 0x80u) {
                crc = (crc << 1u) ^ (CRC7_POLY << 1u);
            } else {
                crc <<= 1u;
            }
        }
    }

    return crc >> 1u;
}

void sd_cmd_prep(sd_cmd_t *buf, uint8_t cmd, uint32_t arg)
{
    /* [byte 0]: Start bit (0) + transmission bit (1) + command index (6)
     * [byte 1-4]: Argument (32 bits)
     * [byte 5]: CRC7 (7 bits) + end bit (1)
     */
    buf->buf[0u] = 0x40u | (cmd & 0x3Fu);
    sys_write_be32(&buf->buf[1u], arg);
    buf->buf[5u] = crc7(&buf->buf[0u], 5u) << 1u | 0x01u;
}