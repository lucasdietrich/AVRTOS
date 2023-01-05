/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_ERRNO_H
#define _AVRTOS_ERRNO_H

#define EERR   0
#define EINTR 4
#define EIO 5
#define EBADF 9
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define EBUSY 16
#define EINVAL 22
#define ENFILE 23
#define ENOMSG 35
#define ENOTSUP 95
#define ETIMEDOUT 110
#define ECANCELED 125

#define EWOULDBLOCK EAGAIN


#endif /* _AVRTOS_ERRNO_H */