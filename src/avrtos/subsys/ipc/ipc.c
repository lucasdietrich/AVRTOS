#include "ipc.h"
#include "link_private.h"

#define IPC_RX_BUFFER_SIZE 64u
#define IPC_TX_BUFFER_SIZE 64u

struct ipc_spi_data {
	struct spi_slave slave;
};

struct ipc_uart_data {
        UART_Device *device;
};

struct ipc_data {
	ipc_link_t link_type : 1u;
	uint8_t initialized : 1u;

        char rx_buf[IPC_RX_BUFFER_SIZE];
        char tx_buf[IPC_TX_BUFFER_SIZE];

        struct k_sem sem;

	union {
		struct ipc_uart_data uart;
		struct ipc_spi_data spi;
	};
};

struct ipc_data ipc_data;

int ipc_init(const struct ipc_config *config)
{
	int ret = 0;

#if CONFIG_KERNEL_ARGS_CHECKS
	if (!config || !config->link) {
		return -EINVAL;
	}
#endif

	if (ipc_data.initialized) {
		goto exit;
	}

	ret = __ipc_link_init(config);	

	if (ret == 0) {
		// ipc_data.link_type   = config->link_type;
		ipc_data.initialized = 1u;
	}

exit:
	return ret;
}

int ipc_deinit(void)
{
	int ret = 0;

	if (!ipc_data.initialized) {
		goto exit;
	}

	__ipc_link_deinit();

	if (ret == 0) {
		ipc_data.initialized = 0u;
	}

exit:
	return ret;
}

int ipc_send(const void *data, size_t size, uint8_t flags)
{
}

int ipc_recv(void *data, size_t size, uint8_t flags)
{
}

int ipc_ioctl(int request, void *data)
{
}

int ipc_poll(void)
{
}