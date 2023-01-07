/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_FD_H
#define _AVRTOS_FD_H

#include <avrtos/dstruct/dlist.h>
#include <avrtos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________*/

struct fd_op_vtable {
	int (*read)(void *obj, void *buf, int len);
	int (*write)(void *obj, const void *buf, int len);
	int (*close)(void *obj);
	int (*ioctl)(void *obj, int req, void *arg);
};

struct fd {
	void *obj;
	atomic_t refcnt;
	const struct fd_op_vtable *vtable;
	struct k_mutex mutex;
};

/*___________________________________________________________________________*/

//
// POSIX API
//

int read(int fd, void *buf, int len);

int write(int fd, const void *buf, int len);

int close(int fd);

int ioctl(int fd, int req, void *arg);

/*___________________________________________________________________________*/

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_FD_H */