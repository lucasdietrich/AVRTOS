/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AVRTOS_DEVICE_MCP2515_H
#define _AVRTOS_DEVICE_MCP2515_H

#include <stddef.h>
#include <stdint.h>

#include <avrtos/drivers/can.h>
#include <avrtos/drivers/spi.h>
#include <avrtos/kernel.h>
#include <avrtos/mutex.h>

/**
 * MCP2515 CAN Controller Driver
 *
 * This driver provides an interface to interact with the MCP2515 CAN controller.
 * It includes functions to initialize, send, receive CAN messages, and set filters
 * and masks. It relies on SPI for communication.
 *
 * Configuration of the MCP2515 includes setting the CAN speed, clock speed,
 * and various flags for debugging and interrupt control.
 *
 * Note that the MCP2515 driver is currently experimental and has the following
 * limitations:
 * - The only supported clock speed is 16 Mhz
 * - The only supported bus speed is 500 kbps
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enum representing possible CAN bus speeds for the MCP2515.
 */
typedef enum mcp2515_can_speed {
    MCP2515_CAN_SPEED_10KBPS  = 0u, /**< 10 kbps */
    MCP2515_CAN_SPEED_20KBPS  = 1u, /**< 20 kbps */
    MCP2515_CAN_SPEED_50KBPS  = 2u, /**< 50 kbps */
    MCP2515_CAN_SPEED_100KBPS = 3u, /**< 100 kbps */
    MCP2515_CAN_SPEED_125KBPS = 4u, /**< 125 kbps */
    MCP2515_CAN_SPEED_250KBPS = 5u, /**< 250 kbps */
    MCP2515_CAN_SPEED_500KBPS = 6u, /**< 500 kbps */
    MCP2515_CAN_SPEED_1MBPS   = 7u  /**< 1 Mbps */
} mcp2515_can_speed_t;

/**
 * @brief Enum representing various flags for the MCP2515 configuration.
 */
typedef enum mcp2515_flags {
    MCP2515_INT_RX            = BIT(0u), /**< Enable interrupt on full RX buffer */
    MCP2515_INT_TX            = BIT(1u), /**< Enable interrupt on empty TX buffer */
    MCP2515_DEBUG_NO_ROLLOVER = BIT(2u), /**< Disable rollover for debugging purposes */
    MCP2515_DEBUG_RX_ANY      = BIT(3u)  /**< Disable filtering for debugging purposes */
} mcp2515_flags_t;

/**
 * @brief Enum representing possible clock speeds for the MCP2515.
 */
typedef enum mcp2515_clock_set {
    MCP2515_CLOCK_SET_8MHZ  = 0u, /**< 8 MHz clock */
    MCP2515_CLOCK_SET_16MHZ = 1u  /**< 16 MHz clock */
} mcp2515_clock_set_t;

/**
 * @brief Configuration structure for the MCP2515 device.
 */
struct mcp2515_config {
    mcp2515_can_speed_t can_speed : 3u;   /**< CAN bus speed */
    mcp2515_clock_set_t clock_speed : 1u; /**< MCP2515 clock speed */
    uint8_t flags : 4u;                   /**< Flags for interrupt and debug options */
};

/**
 * @brief MCP2515 device structure.
 */
struct mcp2515_device {
    struct spi_slave slave;          /**< SPI slave structure */
    struct k_mutex Z_PRIVATE(mutex); /**< Mutex for thread-safe access */
};

/**
 * @brief Initialize the MCP2515 device.
 *
 * This function is not safe to be called from an ISR.
 *
 * @param mcp Pointer to the MCP2515 device structure.
 * @param config Pointer to the MCP2515 configuration structure.
 * @param spi_slave Pointer to the SPI slave structure.
 * @return int8_t 0 on success, negative value on error.
 */
int8_t mcp2515_init(struct mcp2515_device *mcp,
                    const struct mcp2515_config *config,
                    struct spi_slave *spi_slave);

/**
 * @brief Deinitialize the MCP2515 device.
 *
 * This function is not safe to be called from an ISR.
 *
 * @param mcp Pointer to the MCP2515 device structure.
 * @return int8_t 0 on success, negative value on error.
 */
int8_t mcp2515_deinit(struct mcp2515_device *mcp);

/**
 * @brief Send a CAN frame through the MCP2515.
 *
 * This function is not safe to be called from an ISR.
 *
 * @param mcp Pointer to the MCP2515 device structure.
 * @param frame Pointer to the CAN frame to be sent.
 * @return int8_t 0 on success, negative value on error.
 */
int8_t mcp2515_send(struct mcp2515_device *mcp, const struct can_frame *frame);

/**
 * @brief Receive a CAN frame through the MCP2515.
 *
 * This function is not safe to be called from an ISR.
 *
 * @param mcp Pointer to the MCP2515 device structure.
 * @param frame Pointer to the CAN frame structure to store the received frame.
 * @return int8_t 0 on success, negative value on error.
 */
int8_t mcp2515_recv(struct mcp2515_device *mcp, struct can_frame *frame);

/**
 * @brief Set a filter for the MCP2515 CAN controller.
 *
 * This function is not safe to be called from an ISR.
 *
 * @param mcp Pointer to the MCP2515 device structure.
 * @param index Filter index.
 * @param is_ext Flag to indicate if the filter is extended (1 for extended).
 * @param filter Filter value.
 * @return int8_t 0 on success, negative value on error.
 */
int8_t mcp2515_set_filter(struct mcp2515_device *mcp,
                          uint8_t index,
                          uint8_t is_ext,
                          uint32_t filter);

/**
 * @brief Set a mask for the MCP2515 CAN controller.
 *
 * This function is not safe to be called from an ISR.
 *
 * @param mcp Pointer to the MCP2515 device structure.
 * @param index Mask index.
 * @param is_ext Flag to indicate if the mask is extended (1 for extended).
 * @param mask Mask value.
 * @return int8_t 0 on success, negative value on error.
 */
int8_t mcp2515_set_mask(struct mcp2515_device *mcp,
                        uint8_t index,
                        uint8_t is_ext,
                        uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_DEVICE_MCP2515_H */
