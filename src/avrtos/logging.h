#ifndef _AVRTOS_LOGGING_H_
#define _AVRTOS_LOGGING_H_

#include "kernel.h"

#include <stdio.h>

#include <avr/pgmspace.h>

#define LOG_LEVEL_NONE	  0u
#define LOG_LEVEL_ERROR	  1u
#define LOG_LEVEL_WARNING 2u
#define LOG_LEVEL_INFO	  3u
#define LOG_LEVEL_DEBUG	  4u

#define LOG_LEVEL_ERR LOG_LEVEL_ERROR
#define LOG_LEVEL_WRN LOG_LEVEL_WARNING
#define LOG_LEVEL_INF LOG_LEVEL_INFO
#define LOG_LEVEL_DBG LOG_LEVEL_DEBUG

#if CONFIG_LOGGING_SUBSYSTEM

#define _LOG(level, fmt, ...)                                             \
	do {                                                              \
		if ((level) <= (LOG_LEVEL)) {                             \
			printf_P((const char *)PSTR(fmt), ##__VA_ARGS__); \
		}                                                         \
	} while (0)

/* line continuation */
#define _LOG_HEXDUMP(level, data, len)                             \
	do {                                                       \
		if ((level) <= (LOG_LEVEL)) {                      \
			for (unsigned int i = 0; i < (len); i++) { \
				printf_P(PSTR("%02x "), data[i]);  \
			}                                          \
			printf_P(PSTR("\n"));                      \
		}                                                  \
	} while (0)

#else

#define _LOG(level, fmt, ...)
#define _LOG_HEXDUMP(level, data, len)

#endif /* LOGGING_ENABLED */

/* line begin and end */
#define LOG_DBG(fmt, ...) _LOG(4, fmt "\n", ##__VA_ARGS__)
#define LOG_INF(fmt, ...) _LOG(3, fmt "\n", ##__VA_ARGS__)
#define LOG_WRN(fmt, ...) _LOG(2, fmt "\n", ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) _LOG(1, fmt "\n", ##__VA_ARGS__)

#define LOG_DBG_RAW(fmt, ...) _LOG(4, fmt, ##__VA_ARGS__)
#define LOG_INF_RAW(fmt, ...) _LOG(3, fmt, ##__VA_ARGS__)
#define LOG_WRN_RAW(fmt, ...) _LOG(2, fmt, ##__VA_ARGS__)
#define LOG_ERR_RAW(fmt, ...) _LOG(1, fmt, ##__VA_ARGS__)

#define LOG_HEXDUMP_DBG(data, len) _LOG_HEXDUMP(4, data, len)
#define LOG_HEXDUMP_INF(data, len) _LOG_HEXDUMP(3, data, len)
#define LOG_HEXDUMP_WRN(data, len) _LOG_HEXDUMP(2, data, len)
#define LOG_HEXDUMP_ERR(data, len) _LOG_HEXDUMP(1, data, len)

#endif /* _AVRTOS_LOGGING_H_ */