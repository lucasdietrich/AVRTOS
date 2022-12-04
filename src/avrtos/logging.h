#ifndef _AVRTOS_LOGGING_H_
#define _AVRTOS_LOGGING_H_

#include <avrtos/kernel.h>

#include <stdio.h>
#include <avr/pgmspace.h>

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERR 1
#define LOG_LEVEL_WRN 2
#define LOG_LEVEL_INF 3
#define LOG_LEVEL_DBG 4

#if LOGGING_SUBSYSTEM

#define _LOG(level, fmt, ...) \
    do { \
        if ((level) <= (LOG_LEVEL)) { \
            printf_P((const char *) PSTR(fmt), ## __VA_ARGS__); \
        } \
    } while (0)

/* line continuation */
#define _LOG_HEXDUMP(level, data, len) \
   do { \
	if ((level) <= (LOG_LEVEL)) { \
		for(unsigned int i = 0; i < (len); i++) { \
			printf_P(PSTR("%02x "), data[i]); \
		} \
		printf_P(PSTR("\n")); \
	} \
   } while (0)

#else

#define _LOG(level, fmt, ...)
#define _LOG_HEXDUMP(level, data, len)

#endif /* LOGGING_ENABLED */

/* line begin and end */
#define LOG_DBG(fmt, ...) _LOG(4, fmt "\n", ## __VA_ARGS__)
#define LOG_INF(fmt, ...) _LOG(3, fmt "\n", ## __VA_ARGS__)
#define LOG_WRN(fmt, ...) _LOG(2, fmt "\n", ## __VA_ARGS__)
#define LOG_ERR(fmt, ...) _LOG(1, fmt "\n", ## __VA_ARGS__)

#define LOG_DBG_RAW(fmt, ...) _LOG(4, fmt, ## __VA_ARGS__)
#define LOG_INF_RAW(fmt, ...) _LOG(3, fmt, ## __VA_ARGS__)
#define LOG_WRN_RAW(fmt, ...) _LOG(2, fmt, ## __VA_ARGS__)
#define LOG_ERR_RAW(fmt, ...) _LOG(1, fmt, ## __VA_ARGS__)

#define LOG_HEXDUMP_DBG(data, len) _LOG_HEXDUMP(4, data, len)
#define LOG_HEXDUMP_INF(data, len) _LOG_HEXDUMP(3, data, len)
#define LOG_HEXDUMP_WRN(data, len) _LOG_HEXDUMP(2, data, len)
#define LOG_HEXDUMP_ERR(data, len) _LOG_HEXDUMP(1, data, len)

#endif /* _AVRTOS_LOGGING_H_ */