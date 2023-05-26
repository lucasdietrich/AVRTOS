/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Inter processor communication over UART/SPI */

#ifndef _IPC_H
#define _IPC_H

#include <stdint.h>
#include <stddef.h>

#include <avrtos/drivers/usart.h>
#include <avrtos/drivers/spi.h>

typedef enum {
        IPC_LINK_UART,
        IPC_LINK_SPI,
} ipc_link_t;

#define IPC_CFG_SPI_MASTER 0x01

struct ipc_config {
	// union {
	// 	struct {
	// 		const struct usart_config *config;
	// 		UART_Device *device;
	// 	} uart;
	// 	struct {
	// 		const struct spi_config *config;
        //                 const struct spi_slave *slave;
	// 	} spi;
	// };
};

#define IPC_FLAG_NONE	  0x00
#define IPC_FLAG_BLOCKING 0x01

#define IPC_IOCTL_SET_SPI_FREQ   0x01
#define IPC_IOCTL_GET_SPI_FREQ   0x02

int ipc_init(const struct ipc_config *config);

int ipc_deinit(void);

int ipc_send(const void *data, size_t size, uint8_t flags);

int ipc_recv(void *data, size_t size, uint8_t flags);

int ipc_ioctl(int request, void *data);

int ipc_poll(void);

#endif /* _IPC_H */