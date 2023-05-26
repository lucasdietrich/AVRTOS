#include "link_private.h"

#if CONFIG_IPC_SPI
static int ipc_spi_link_init(const struct ipc_config *config)
{
	spi_init(config);
	// spi_slave_ss_init(config->spi.slave);
}

static int ipc_spi_deinit(void)
{
	spi_deinit();
}

static int ipc_link_spi_init(void *cfg)
{
        (void) cfg;

        return 0;
}

static int ipc_link_spi_deinit(void)
{
        return 0;
}

int __ipc_link_init(void *cfg) __attribute__ ((weak, alias ("ipc_link_spi_init")));
int __ipc_link_deinit(void) __attribute__ ((weak, alias ("ipc_link_spi_deinit")));

#endif