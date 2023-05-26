#ifndef _IPC_LINK_PRIVATE_H
#define _IPC_LINK_PRIVATE_H

#include <avrtos/subsys/ipc/ipc.h>

int __ipc_link_init(void *cfg);

int __ipc_link_deinit(void);

#endif /* _IPC_LINK_PRIVATE_H */