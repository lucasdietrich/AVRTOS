/*
 * Copyright (c) 2022 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>
#include <avrtos/devices/mcp2515.h>
#include <avrtos/drivers/can.h>
#include <avrtos/drivers/exti.h>
#include <avrtos/drivers/gpio.h>
#include <avrtos/drivers/spi.h>

static struct mcp2515_device mcp;

K_SEM_DEFINE(can_int_sem, 0u, 10u);

ISR(INT0_vect)
{
    serial_transmit('!');
    k_yield_from_isr_cond(k_sem_give(&can_int_sem));
}

static void print_can_frame(struct can_frame *frame)
{
    // oneline
    printf("id: %x%x, len: %d, rtr: %d, is_ext: %d / ", (uint16_t)(frame->id >> 16),
           (uint16_t)frame->id, frame->len, frame->rtr, frame->is_ext);
    for (uint8_t i = 0; i < frame->len; i++) {
        printf(" 0x%02X ", frame->data[i]);
    }
    printf("\n");
}

int main(void)
{
    int ret;
    serial_init_baud(115200u);

    const struct spi_config spi_cfg = {
        .role        = SPI_ROLE_MASTER,
        .polarity    = SPI_CLOCK_POLARITY_RISING,
        .phase       = SPI_CLOCK_PHASE_SAMPLE,
        .prescaler   = SPI_PRESCALER_4,
        .irq_enabled = 0u,
    };

    struct spi_slave spi_slave = {
        .cs_port      = GPIOB_DEVICE,
        .cs_pin       = 2u,
        .active_state = GPIO_LOW,
        .regs         = spi_config_into_regs(spi_cfg),
    };

    gpio_pin_init(GPIOB_DEVICE, 1u, GPIO_MODE_OUTPUT, GPIO_HIGH);

    const struct mcp2515_config mcp_cfg = {
        .can_speed   = MCP2515_CAN_SPEED_500KBPS,
        .clock_speed = MCP2515_CLOCK_SET_16MHZ,
        .flags       = MCP2515_INT_RX, // MCP2515_INT_TX
    };

    spi_init(spi_cfg);

    ret = mcp2515_init(&mcp, &mcp_cfg, &spi_slave);
    printf("mcp2515_init: %d\n", ret);

    mcp2515_set_mask(&mcp, 0u, CAN_STD_ID, 0x3FF);
    mcp2515_set_filter(&mcp, 0u, CAN_STD_ID, 0x0000);
    mcp2515_set_filter(&mcp, 1u, CAN_STD_ID, 0x0000);

    mcp2515_set_mask(&mcp, 1u, CAN_STD_ID, 0x123lu);
    mcp2515_set_filter(&mcp, 2u, CAN_STD_ID, 0x123lu);
    mcp2515_set_filter(&mcp, 3u, CAN_STD_ID, 0x123lu);
    mcp2515_set_filter(&mcp, 4u, CAN_STD_ID, 0x123lu);
    mcp2515_set_filter(&mcp, 5u, CAN_STD_ID, 0x123lu);

    gpio_pin_init(GPIOD_DEVICE, 2u, GPIO_MODE_INPUT, GPIO_INPUT_PULLUP);

    exti_clear_flag(INT0);
    exti_configure(INT0, ISC_FALLING);
    exti_enable(INT0);

    for (;;) {
        struct can_frame frame = {0};

        k_sem_take(&can_int_sem, K_FOREVER);

        int8_t ret = mcp2515_recv(&mcp, &frame);
        printf("mcp2515_recv: %d\n", ret);

        if (ret == 0) {
            print_can_frame(&frame);

            // Echo received frame back
            ret = mcp2515_send(&mcp, &frame);
            printf("mcp2515_send: %d\n", ret);
        }
    }

    ret = mcp2515_deinit(&mcp);
    printf("mcp2515_deinit: %d\n", ret);

    k_sleep(K_FOREVER);
}