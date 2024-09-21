/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_SUBSYS_IOCTL_H
#define _AVRTOS_SUBSYS_IOCTL_H

#include <stddef.h>
#include <stdint.h>

#define F_GETFL 0x01
#define F_SETFL 0x02

#define O_NONBLOCK 0x01
#define O_R		   0x02
#define O_W		   0x04

#endif /* _AVRTOS_SUBSYS_IOCTL_H */