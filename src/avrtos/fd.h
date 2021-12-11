#ifndef _AVRTOS_FD_H
#define _AVRTOS_FD_H

#include <avrtos/kernel.h>
#include <avrtos/dstruct/dlist.h>

#ifdef __cplusplus
extern "C" {
#endif


/*___________________________________________________________________________*/

struct fd_op_vtable
{
        int (*read)(void *obj, void *buf, int len);
        int (*write)(void *obj, const void *buf, int len);
        int (*close)(void *obj);
        int (*ioctl)(void *obj, int req, void *arg);
};

struct fd
{
        void *obj;
        atomic_t refcnt;
        const struct fd_op_vtable *vtable;
        struct k_mutex mutex;
};

int z_fd_reserve(void);

void z_fd_finalize(int fd, void *obj, const struct fd_op_vtable *vtable);

int z_fd_alloc(void *obj, const struct fd_op_vtable *vtable);

void z_fd_free(int fd);


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