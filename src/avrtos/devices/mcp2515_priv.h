/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEVICE_MCP2515_PRIV_H
#define _AVRTOS_DEVICE_MCP2515_PRIV_H

#include <stddef.h>
#include <stdint.h>

// SPI Intruction set (TABLE 12-1: SPI INSTRUCTION SET)
#define MCP_RESET                   0xC0
#define MCP_READ                    0x03
#define MCP_READ_RX_BUFFER(n, m)    (0x90 | ((n) << 2) | ((m) << 1))
#define MCP_WRITE                   0x02
#define MCP_LOAD_TX_BUFFER(a, b, c) (0x40 | ((a) << 5) | ((b) << 6) | ((c) << 7))
#define MCP_RTS(tx0, tx1, tx2)      (0x80 | (tx0) | ((tx1) << 1) | ((tx2) << 2))
#define MCP_READ_STATUS             0xA0
#define MCP_RX_STATUS               0xB0
#define MCP_BIT_MODIFY              0x05

#define MCP_READ_RX_AT_RXB0SIDH MCP_READ_RX_BUFFER(0, 0)
#define MCP_READ_RX_AT_RXB0D0   MCP_READ_RX_BUFFER(0, 1)
#define MCP_READ_RX_AT_RXB1SIDH MCP_READ_RX_BUFFER(1, 0)
#define MCP_READ_RX_AT_RXB1D0   MCP_READ_RX_BUFFER(1, 1)

#define MCP_LOAD_TX_AT_TXB0SIDH MCP_LOAD_TX_BUFFER(0, 0, 0)
#define MCP_LOAD_TX_AT_TXB0D0   MCP_LOAD_TX_BUFFER(0, 0, 1)
#define MCP_LOAD_TX_AT_TXB1SIDH MCP_LOAD_TX_BUFFER(0, 1, 0)
#define MCP_LOAD_TX_AT_TXB1D0   MCP_LOAD_TX_BUFFER(0, 1, 1)
#define MCP_LOAD_TX_AT_TXB2SIDH MCP_LOAD_TX_BUFFER(1, 0, 0)
#define MCP_LOAD_TX_AT_TXB2D0   MCP_LOAD_TX_BUFFER(1, 0, 1)

#define MCP_RTS_TXB0 MCP_RTS(1, 0, 0)
#define MCP_RTS_TXB1 MCP_RTS(0, 1, 0)
#define MCP_RTS_TXB2 MCP_RTS(0, 0, 1)
#define MCP_RTS_ALL  MCP_RTS(1, 1, 1)

// RXB Buffers offsets and masks
#define MCP_RXB_SIDH 0
#define MCP_RXB_SIDL 1
#define MCP_RXB_EID8 2
#define MCP_RXB_EID0 3

#define MCP_RXB_SIDL_SRR_MASK 0x10
#define MCP_RXB_SIDL_IDE_MASK 0x08

#define MCP_RXB_DLC_DLC_MASK 0x0F
#define MCP_RXB_DLC_RTR_MASK 0x40

// registers

#define MCP_R_CANSTAT 0x0E
#define MCP_R_CANCTRL 0x0F
#define MCP_R_CANINTE 0x2B
#define MCP_R_CANINTF 0x2C
#define MCP_R_CNF3    0x28
#define MCP_R_CNF2    0x29
#define MCP_R_CNF1    0x2A

#define MCP_R_TXB0CTRL 0x30
#define MCP_R_TXB1CTRL 0x40
#define MCP_R_TXB2CTRL 0x50

#define MCP_R_TXB0SIDH 0x31
#define MCP_R_TXB1SIDH 0x41
#define MCP_R_TXB2SIDH 0x51

#define MCP_R_RXB0CTRL 0x60
#define MCP_R_RXB1CTRL 0x70

#define MCP_R_RXB0SIDH 0x61
#define MCP_R_RXB1SIDH 0x71

#define MCP_R_RXF0SIDH 0x00
#define MCP_R_RXF1SIDH 0x04
#define MCP_R_RXF2SIDH 0x08
#define MCP_R_RXF3SIDH 0x10
#define MCP_R_RXF4SIDH 0x14
#define MCP_R_RXF5SIDH 0x18

#define MCP_R_RXM0SIDH 0x20
#define MCP_R_RXM1SIDH 0x24

// RXB0CTRL Register Bits/Masks

#define MCP_RXB0CTRL_RXM_MASK   0x60
#define MCP_RXB0CTRL_RXM_ANY    MCP_RXB0CTRL_RXM_MASK
#define MCP_RXB0CTRL_RXM_STDEXT 0x00

#define MCP_RXB0CTRL_BUKT_MASK        0x04
#define MCP_RXB0CTRL_BUKT_ROLLOVER    MCP_RXB0CTRL_BUKT_MASK
#define MCP_RXB0CTRL_BUKT_NO_ROLLOVER 0x00

// RXB1CTRL Register Bits/Masks

#define MCP_RXB1CTRL_RXM_MASK   0x60
#define MCP_RXB1CTRL_RXM_ANY    MCP_RXB1CTRL_RXM_MASK
#define MCP_RXB1CTRL_RXM_STDEXT 0x00

// READ STATUS Register Bits

#define MCP_STATUS_RX0IF  0x01
#define MCP_STATUS_RX1IF  0x02
#define MCP_STATUS_TX0REQ 0x04
#define MCP_STATUS_TX0IF  0x08
#define MCP_STATUS_TX1REQ 0x10
#define MCP_STATUS_TX1IF  0x20
#define MCP_STATUS_TX2REQ 0x40
#define MCP_STATUS_TX2IF  0x80

#define MCP_STATUS_RXIF_MASK  (MCP_STATUS_RX0IF | MCP_STATUS_RX1IF)
#define MCP_STATUS_TXREQ_MASK (MCP_STATUS_TX0REQ | MCP_STATUS_TX1REQ | MCP_STATUS_TX2REQ)

// CANINTE Register Bits

#define MCP_CANINTE_RX0IE 0x01
#define MCP_CANINTE_RX1IE 0x02
#define MCP_CANINTE_TX0IE 0x04
#define MCP_CANINTE_TX1IE 0x08
#define MCP_CANINTE_TX2IE 0x10
#define MCP_CANINTE_ERRIE 0x20
#define MCP_CANINTE_WAKIE 0x40
#define MCP_CANINTE_MERRE 0x80

// CANINTF Register Bits

#define MCP_CANINTF_RX0IF 0x01
#define MCP_CANINTF_RX1IF 0x02
#define MCP_CANINTF_TX0IF 0x04
#define MCP_CANINTF_TX1IF 0x08
#define MCP_CANINTF_TX2IF 0x10
#define MCP_CANINTF_ERRIF 0x20
#define MCP_CANINTF_WAKIF 0x40
#define MCP_CANINTF_MERRF 0x80

// CANCTRL Register Bits
#define MCP_MASK_MODE 0xE0

#define MCP_MODE_NORMAL      0x00
#define MCP_MODE_SLEEP       0x20
#define MCP_MODE_LOOPBACK    0x40
#define MCP_MODE_LISTEN_ONLY 0x60
#define MCP_MODE_CONFIG      0x80

// speed and clock configuration
#define MCP_16MHz_500kBPS_CFG1 (0x00)
#define MCP_16MHz_500kBPS_CFG2 (0xF0)
#define MCP_16MHz_500kBPS_CFG3 (0x86)

#endif /* _AVRTOS_DEVICE_MCP2515_PRIV_H */