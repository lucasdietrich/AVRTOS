#include "link_private.h"

#if CONFIG_IPC_UART
static int ipc_uart_link_init(const struct ipc_config *config)
{
	ipc_data.uart.device = config->uart.device;
	ll_usart_init(config->uart.device, config->uart.config);
}

static int ipc_uart_deinit(void)
{
	ll_usart_deinit(ipc_data.uart.device);
}
#endif