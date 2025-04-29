/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mcp2515.h"
#include <stdint.h>
#include <string.h>

#include <avrtos/logging.h>

#include "mcp2515_priv.h"

#if CONFIG_DEVICE_MCP2515

#warning "MCP2515 driver is experimental and has limitations"

#define K_MODULE  K_MODULE_DEVICE
#define LOG_LEVEL LOG_LEVEL_INF

#if !defined(CONFIG_MCP2515_DELAY_MS)
#define CONFIG_MCP2515_DELAY_MS 10u
#endif

#define MCP_SELECT(_dev)   spi_slave_select(&_dev->slave);
#define MCP_UNSELECT(_dev) spi_slave_unselect(&_dev->slave);

#define spi_read() spi_transceive(0x00)

static void write_register(struct mcp2515_device *mcp, uint8_t reg_addr, uint8_t reg_val)
{
    MCP_SELECT(mcp);
    spi_transceive(MCP_WRITE);
    spi_transceive(reg_addr);
    spi_transceive(reg_val);
    MCP_UNSELECT(mcp);
}

static void write_registers(struct mcp2515_device *mcp,
                            uint8_t reg_addr,
                            const uint8_t *buf,
                            size_t len)
{
    MCP_SELECT(mcp);
    spi_transceive(MCP_WRITE);
    spi_transceive(reg_addr);
    while (len--) {
        spi_transceive(*buf++);
    }
    MCP_UNSELECT(mcp);
}

static void
modify_register(struct mcp2515_device *mcp, uint8_t reg_addr, uint8_t mask, uint8_t data)
{
    MCP_SELECT(mcp);
    spi_transceive(MCP_BIT_MODIFY);
    spi_transceive(reg_addr);
    spi_transceive(mask);
    spi_transceive(data);
    MCP_UNSELECT(mcp);
}

static void set_mode(struct mcp2515_device *mcp, uint8_t new_mode)

{
    modify_register(mcp, MCP_R_CANINTF, MCP_CANINTF_WAKIF, 0);
    modify_register(mcp, MCP_R_CANCTRL, MCP_MASK_MODE, new_mode);
}

static inline uint8_t read_status(struct mcp2515_device *mcp)
{
    MCP_SELECT(mcp);
    spi_transceive(MCP_READ_STATUS);
    uint8_t status = spi_read();
    MCP_UNSELECT(mcp);
    return status;
}

static void mcp_reset(struct mcp2515_device *mcp)
{
    MCP_SELECT(mcp);
    spi_transceive(MCP_RESET);
    MCP_UNSELECT(mcp);
}

static int8_t config_dr(struct mcp2515_device *mcp,
                        mcp2515_can_speed_t speed,
                        mcp2515_clock_set_t clock)
{
    // only support MCP_16MHz and CAN_500KBPS for now
    if (speed != MCP2515_CAN_SPEED_500KBPS || clock != MCP2515_CLOCK_SET_16MHZ) {
        return -ENOTSUP;
    }

    uint8_t cfg1 = MCP_16MHz_500kBPS_CFG1;
    uint8_t cfg2 = MCP_16MHz_500kBPS_CFG2;
    uint8_t cfg3 = MCP_16MHz_500kBPS_CFG3;

    write_register(mcp, MCP_R_CNF1, cfg1);
    write_register(mcp, MCP_R_CNF2, cfg2);
    write_register(mcp, MCP_R_CNF3, cfg3);

    return 0;
}

int8_t mcp2515_init(struct mcp2515_device *mcp,
                    const struct mcp2515_config *config,
                    struct spi_slave *spi_slave)
{
    int8_t ret;

    /* Init device instance */
    memcpy(&mcp->slave, spi_slave, sizeof(struct spi_slave));
    k_mutex_init(&mcp->_mutex);

    /* Set SPI */
    spi_regs_restore(&spi_slave->regs);
    spi_slave_ss_init(&mcp->slave);

    /* Reset the MCP2515 */
    mcp_reset(mcp);
    k_msleep(CONFIG_MCP2515_DELAY_MS); // wait for reset

    /* Enter configuration mode */
    set_mode(mcp, MCP_MODE_CONFIG);
    k_msleep(CONFIG_MCP2515_DELAY_MS); // wait for mode change

    /* Configure CAN bus speed depending on the clock speed */
    ret = config_dr(mcp, config->can_speed, config->clock_speed);
    if (ret) {
        LOG_ERR("config_dr failed: %d", ret);
        return ret;
    }
    k_msleep(CONFIG_MCP2515_DELAY_MS); // wait for config

    /* Init TX buffers
     * Check TXBnCTRL to TXBnDLC
     */
    uint8_t tx1 = MCP_R_TXB0CTRL;
    uint8_t tx2 = MCP_R_TXB1CTRL;
    uint8_t tx3 = MCP_R_TXB2CTRL;

    for (uint_fast8_t i = 0; i < 14; i++, tx1++, tx2++, tx3++) {
        write_register(mcp, tx1, 0);
        write_register(mcp, tx2, 0);
        write_register(mcp, tx3, 0);
    }

    /* Init RX buffers */
    write_register(mcp, MCP_R_RXB0CTRL, 0);
    write_register(mcp, MCP_R_RXB1CTRL, 0);

    /* Configure interrupts */
    uint8_t inte = 0;
    if (config->flags & MCP2515_INT_RX) {
        inte |= MCP_CANINTE_RX0IE | MCP_CANINTE_RX1IE;
    }

    if (config->flags & MCP2515_INT_TX) {
        inte |= MCP_CANINTE_TX0IE | MCP_CANINTE_TX1IE | MCP_CANINTE_TX2IE;
    }

    write_register(mcp, MCP_R_CANINTE, inte);

    /* Enable both RX buffers and enable rollover */
    uint8_t rxb0ctrl = 0;
    uint8_t rxb1ctrl = 0;

    if (config->flags & MCP2515_DEBUG_RX_ANY) {
        rxb0ctrl |= MCP_RXB0CTRL_RXM_ANY;
        rxb1ctrl |= MCP_RXB1CTRL_RXM_ANY;
    } else {
        rxb0ctrl |= MCP_RXB0CTRL_RXM_STDEXT;
        rxb1ctrl |= MCP_RXB1CTRL_RXM_STDEXT;
    }

    rxb0ctrl |= config->flags & MCP2515_DEBUG_NO_ROLLOVER ? MCP_RXB0CTRL_BUKT_NO_ROLLOVER
                                                          : MCP_RXB0CTRL_BUKT_ROLLOVER;
    modify_register(mcp, MCP_R_RXB0CTRL, MCP_RXB0CTRL_RXM_MASK | MCP_RXB0CTRL_BUKT_MASK,
                    rxb0ctrl);
    modify_register(mcp, MCP_R_RXB1CTRL, MCP_RXB1CTRL_RXM_MASK, rxb1ctrl);

    /* Enter normal mode */
    set_mode(mcp, MCP_MODE_NORMAL);
    k_msleep(CONFIG_MCP2515_DELAY_MS); // wait for mode change

    return ret;
}

int8_t mcp2515_deinit(struct mcp2515_device *mcp)
{
    k_mutex_lock(&mcp->_mutex, K_FOREVER);

    mcp_reset(mcp);

    k_mutex_cancel_wait(&mcp->_mutex);

    memset(mcp, 0, sizeof(struct mcp2515_device));

    return 0;
}

static int8_t
read_rx_buf(struct mcp2515_device *mcp, struct can_frame *frame, uint8_t rxb_reg)
{
    MCP_SELECT(mcp);
    spi_transceive(rxb_reg);

    uint8_t id[4];
    uint8_t dlc;

    for (uint8_t i = 0; i < 4; i++) {
        id[i] = spi_read();
    }
    dlc = spi_read();

    frame->len = MIN(dlc & MCP_RXB_DLC_DLC_MASK, 8u);
    frame->id  = (id[MCP_RXB_SIDH] << 3) | (id[MCP_RXB_SIDL] >> 5); // sid 0-11
    frame->rtr = (dlc & MCP_RXB_DLC_RTR_MASK) ? 1 : 0;
    if ((id[MCP_RXB_SIDL] & MCP_RXB_SIDL_IDE_MASK) != 0) {
        frame->is_ext = 1;
        frame->id     = (frame->id << 2) | (id[MCP_RXB_SIDL] & 0x03); // eid 16-17
        frame->id     = (frame->id << 8) | id[MCP_RXB_EID8];          // eid 8-15
        frame->id     = (frame->id << 8) | id[MCP_RXB_EID0];          // eid 0-7
    } else {
        frame->is_ext = 0;
    }

    for (uint8_t i = 0; i < frame->len; i++) {
        frame->data[i] = spi_read();
    }

    MCP_UNSELECT(mcp);

    return 0;
}

static void start_transmit(struct mcp2515_device *mcp, uint8_t reg_rts)
{
    MCP_SELECT(mcp);
    spi_transceive(reg_rts);
    MCP_UNSELECT(mcp);
}

// buf is a 4-byte buffer
static void can_id_to_buf(uint8_t *buf, uint32_t id, uint8_t is_ext)
{
    __ASSERT_NOTNULL(buf);

    uint16_t half = (uint16_t)id;

    if (is_ext) {
        buf[MCP_RXB_EID0] = half;      // eid 0-7
        buf[MCP_RXB_EID8] = half >> 8; // eid 8-15
        half              = id >> 16;
        buf[MCP_RXB_SIDL] = half & 0x03; // eid 16-17
        buf[MCP_RXB_SIDL] |= MCP_RXB_SIDL_IDE_MASK;
        buf[MCP_RXB_SIDL] |= (half << 3) & 0xE0; // sid 0-2
        buf[MCP_RXB_SIDH] = half >> 5;           // sid 3-10
    } else {
        buf[MCP_RXB_EID0] = 0;
        buf[MCP_RXB_EID8] = 0;
        buf[MCP_RXB_SIDL] = (half << 5) & 0xE0; // sid 0-2
        buf[MCP_RXB_SIDH] = half >> 3;          // sid 3-10
    }
}

int8_t mcp2515_send(struct mcp2515_device *mcp, const struct can_frame *frame)
{
    __ASSERT_NOTNULL(mcp);
    __ASSERT_NOTNULL(frame);

    k_mutex_lock(&mcp->_mutex, K_FOREVER);

    int8_t ret    = 0;
    uint8_t txreq = read_status(mcp) & MCP_STATUS_TXREQ_MASK;
    uint8_t reg_txnif;  // TXnIF flag to clear in CANINTF
    uint8_t reg_txload; // Register to load TX buffer
    uint8_t reg_txrts;  // Register to start transmission

    /* Find a free TX buffer */
    if ((txreq & MCP_STATUS_TX0REQ) == 0) {
        reg_txnif  = MCP_CANINTF_TX0IF;
        reg_txload = MCP_LOAD_TX_AT_TXB0SIDH;
        reg_txrts  = MCP_RTS_TXB0;
    } else if ((txreq & MCP_STATUS_TX1REQ) == 0) {
        reg_txnif  = MCP_CANINTF_TX1IF;
        reg_txload = MCP_LOAD_TX_AT_TXB1SIDH;
        reg_txrts  = MCP_RTS_TXB1;
    } else if ((txreq & MCP_STATUS_TX2REQ) == 0) {
        reg_txnif  = MCP_CANINTF_TX2IF;
        reg_txload = MCP_LOAD_TX_AT_TXB2SIDH;
        reg_txrts  = MCP_RTS_TXB2;
    } else {
        /* All TX buffers are busy */
        ret = -EBUSY;
        goto exit;
    }

    /* Clear TXnIF flag */
    modify_register(mcp, MCP_R_CANINTF, reg_txnif, 0);

    /* Build TX identifier buffer */
    uint8_t id[4];
    can_id_to_buf(id, frame->id, frame->is_ext);

    /* Build DLC */
    uint8_t len = MIN(frame->len, 8u);
    uint8_t dlc = len | (frame->rtr ? MCP_RXB_DLC_RTR_MASK : 0);

    /* Load TX buffer */
    MCP_SELECT(mcp);
    spi_transceive(reg_txload);

    for (uint8_t i = 0; i < 4; i++) {
        spi_transceive(id[i]);
    }
    spi_transceive(dlc);

    for (uint8_t i = 0; i < len; i++) {
        spi_transceive(frame->data[i]);
    }
    MCP_UNSELECT(mcp);

    start_transmit(mcp, reg_txrts);

exit:
    k_mutex_unlock(&mcp->_mutex);

    return ret;
}

int8_t mcp2515_recv(struct mcp2515_device *mcp, struct can_frame *frame)
{
    __ASSERT_NOTNULL(mcp);
    __ASSERT_NOTNULL(frame);

    k_mutex_lock(&mcp->_mutex, K_FOREVER);

    int8_t ret;
    uint8_t status = read_status(mcp);

    LOG_DBG("status: 0x%02X", status);

    if (status & MCP_STATUS_RX0IF) {
        read_rx_buf(mcp, frame, MCP_READ_RX_AT_RXB0SIDH);
        modify_register(mcp, MCP_R_CANINTF, MCP_CANINTF_RX0IF, 0);
        ret = 0;
    } else if (status & MCP_STATUS_RX1IF) {
        read_rx_buf(mcp, frame, MCP_READ_RX_AT_RXB1SIDH);
        modify_register(mcp, MCP_R_CANINTF, MCP_CANINTF_RX1IF, 0);
        ret = 0;
    } else {
        ret = -ENOMSG;
    }

    k_mutex_unlock(&mcp->_mutex);

    return ret;
}

int8_t mcp2515_set_filter(struct mcp2515_device *mcp,
                          uint8_t index,
                          uint8_t is_ext,
                          uint32_t filter)
{
    __ASSERT_NOTNULL(mcp);

    /* Support 6 filters */
    if (index > 5) {
        return -EINVAL;
    }

    k_mutex_lock(&mcp->_mutex, K_FOREVER);

    set_mode(mcp, MCP_MODE_CONFIG);
    k_msleep(CONFIG_MCP2515_DELAY_MS);

    uint8_t id[4];
    can_id_to_buf(id, filter, is_ext);

    switch (index) {
    case 0:
        write_registers(mcp, MCP_R_RXF0SIDH, id, 4);
        break;
    case 1:
        write_registers(mcp, MCP_R_RXF1SIDH, id, 4);
        break;
    case 2:
        write_registers(mcp, MCP_R_RXF2SIDH, id, 4);
        break;
    case 3:
        write_registers(mcp, MCP_R_RXF3SIDH, id, 4);
        break;
    case 4:
        write_registers(mcp, MCP_R_RXF4SIDH, id, 4);
        break;
    case 5:
        write_registers(mcp, MCP_R_RXF5SIDH, id, 4);
        break;
    default:
        break;
    }

    set_mode(mcp, MCP_MODE_NORMAL);
    k_msleep(CONFIG_MCP2515_DELAY_MS);

    k_mutex_unlock(&mcp->_mutex);

    return 0;
}

int8_t
mcp2515_set_mask(struct mcp2515_device *mcp, uint8_t index, uint8_t is_ext, uint32_t mask)
{
    __ASSERT_NOTNULL(mcp);

    /* Support 2 masks  */
    if (index > 1) {
        return -EINVAL;
    }

    k_mutex_lock(&mcp->_mutex, K_FOREVER);

    set_mode(mcp, MCP_MODE_CONFIG);
    k_msleep(CONFIG_MCP2515_DELAY_MS);

    uint8_t id[4];
    can_id_to_buf(id, mask, is_ext);

    switch (index) {
    case 0:
        write_registers(mcp, MCP_R_RXM0SIDH, id, 4);
        break;
    case 1:
        write_registers(mcp, MCP_R_RXM1SIDH, id, 4);
        break;
    default:
        break;
    }

    set_mode(mcp, MCP_MODE_NORMAL);
    k_msleep(CONFIG_MCP2515_DELAY_MS);

    k_mutex_unlock(&mcp->_mutex);

    return 0;
}

#endif /* CONFIG_DEVICE_MCP2515 */