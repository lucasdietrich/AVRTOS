#include "fd.h"

K_MUTEX_DEFINE(fd_table_mutex);

static struct fd fd_table[FD_MAX_COUNT];

static int fd_check(int fd)
{
        if (fd < 0 || fd >= FD_MAX_COUNT) {
                return -EBADF;
        }

        if (atomic_get(&fd_table[fd].refcnt) != 0) {
                return -EBADF;
        }

        return 0;
}

static int fd_ref(int fd)
{
        return atomic_inc(&fd_table[fd].refcnt);
}

static int fd_unref(int fd)
{
        /* naively set refcounter to 0 */
        if (atomic_clear(&fd_table[fd].refcnt) != 0) {
                fd_table[fd].obj = NULL;
                fd_table[fd].vtable = NULL;
        }
        return 0;
}

static int find_entry(void)
{
        for (struct fd *p = fd_table; p < &fd_table[FD_MAX_COUNT]; p++) {
                if (atomic_get(&p->refcnt) == 0) {
                        return p - fd_table;
                }
        }
        return -ENFILE;
}

int z_fd_reserve(void)
{
        k_mutex_lock(&fd_table_mutex, K_FOREVER);

        int fd = find_entry();
        if (fd >= 0) {
                fd_table[fd].obj = NULL;
                fd_table[fd].vtable = NULL;

                fd_ref(fd);
        }

        k_mutex_unlock(&fd_table_mutex);

        return fd;
}

void z_fd_finalize(int fd, void *obj, const struct fd_op_vtable *vtable)
{
        fd_table[fd].obj = obj;
        fd_table[fd].vtable = vtable;

        k_mutex_init(&fd_table[fd].mutex);
}

int z_fd_alloc(void *obj, const struct fd_op_vtable *vtable)
{
        int fd = z_fd_reserve();
        if (fd >= 0) {
                z_fd_finalize(fd, obj, vtable);
        }

        return fd;
}

void z_fd_free(int fd)
{
        fd_unref(fd);
}

/*___________________________________________________________________________*/

int read(int fd, void *buf, int len)
{
        int ret;

        ret = fd_check(fd);
        if (ret < 0) {
                return ret;
        }

        return fd_table[fd].vtable->read(fd_table[fd].obj, buf, len);
}

int write(int fd, const void *buf, int len)
{
        int ret;

        ret = fd_check(fd);
        if (ret < 0) {
                return ret;
        }

        return fd_table[fd].vtable->write(fd_table[fd].obj, buf, len);
}

int close(int fd)
{
        int ret;

        ret = fd_check(fd);
        if (ret < 0) {
                return ret;
        }

        ret = fd_table[fd].vtable->close(fd_table[fd].obj);

        fd_unref(fd);

        return ret;
}

int ioctl(int fd, int req, void *arg)
{
        int ret;

        ret = fd_check(fd);
        if (ret < 0) {
                return ret;
        }

        return fd_table[fd].vtable->ioctl(fd_table[fd].obj, req, arg);
}