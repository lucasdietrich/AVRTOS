/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_ERRNO_H
#define _AVRTOS_ERRNO_H

/*
 * Error Codes (errno.h)
 *
 * This header file defines a set of standard error codes.
 * The error codes *mostly* follow the values of standard POSIX error codes.
 */

/* EIO (5): Input/output error.
 * This error indicates a generic I/O error. It is often returned when a low-level I/O
 * operation fails, typically in a driver.
 */
#define EIO 5

/* EBADF (9): Bad file descriptor.
 * This error is returned when an invalid identifier is used. It can be an index or an
 * address to a file or resource.
 */
#define EBADF 9

/* EAGAIN (11): Try again.
 * This error indicates that a resource is temporarily unavailable, but the operation
 * may succeed if retried.
 */
#define EAGAIN 11

/* ENOMEM (12): Out of memory.
 * This error occurs when a memory allocation request cannot be fulfilled due to
 * insufficient memory (e.g., msgq, mem_slab).
 */
#define ENOMEM 12

/* EBUSY (16): Device or resource busy.
 * This error is returned when a device or resource is currently in use and cannot
 * be accessed or modified (e.g., workqueue, drivers).
 */
#define EBUSY 16

/* EINVAL (22): Invalid argument.
 * This error indicates that an invalid argument was passed to a function.
 */
#define EINVAL 22

/* ENOMSG (35): No message.
 * This error is returned when a message is not available in the message queue.
 */
#define ENOMSG 35

/* ENOTSUP (95): Operation not supported.
 * This error indicates that the operation is not supported or implemented.
 */
#define ENOTSUP 95

/* EALREADY (114): Operation already in progress.
 * This error occurs when an operation is already in progress and cannot be started
 * again.
 */
#define EALREADY 114

/* ETIMEDOUT (110): Operation timed out.
 * This error indicates that an operation timed out before completion.
 * This error will often appear when a synchronization primitive does not complete
 * within a specified timeout.
 */
#define ETIMEDOUT 110

/* ECANCELED (125): Operation canceled.
 * This error is returned when an operation is explicitly canceled before it is
 * completed. This error will often appear when a synchronization primitive is
 * canceled before completion by a k_*object*_*cancel() function.
 */
#define ECANCELED 125

/* Other error codes, without detailed comments */
#define EPERM		1
#define ENOENT		2
#define ESRCH		3
#define EINTR		4
#define ENXIO		6
#define E2BIG		7
#define ENOEXEC		8
#define ECHILD		10
#define EACCES		13
#define EFAULT		14
#define ENOTBLK		15
#define EEXIST		17
#define EXDEV		18
#define ENODEV		19
#define ENOTDIR		20
#define EISDIR		21
#define ENFILE		23
#define EMFILE		24
#define ENOTTY		25
#define ETXTBSY		26
#define EFBIG		27
#define ENOSPC		28
#define ESPIPE		29
#define EROFS		30
#define EMLINK		31
#define EPIPE		32
#define EDOM		33
#define ERANGE		34
#define EINPROGRESS 115
#define EWOULDBLOCK EAGAIN

/* Maximum error number SHALL hold in an int8_t, so cannot be greater than 127 */

#endif /* _AVRTOS_ERRNO_H */
