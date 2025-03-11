#!/usr/bin/sh

bindgen src/avrtos_all.h \
    --no-layout-tests \
    --use-core \
    --formatter rustfmt \
    --default-enum-style moduleconsts \
    --generate functions,types,methods,vars \
    --explicit-padding \
    --no-size_t-is-usize \
    --allowlist-function k_.* \
    --allowlist-function z_.* \
    --allowlist-function dlist_.* \
    --allowlist-function slist.* \
    --allowlist-function tqueue_.* \
    --allowlist-function led_.* \
    --allowlist-function serial_.* \
    --allowlist-function atomic_.* \
    --allowlist-function __fault \
    --allowlist-function __assert \
    --allowlist-function print_slist \
    --allowlist-function print_dlist \
    --allowlist-function print_tqueue \
    --allowlist-function gpio_.* \
    --allowlist-function i2c_.* \
    --allowlist-function spi_.* \
    --allowlist-function ll_.* \
    --allowlist-function timer_.* \
    --allowlist-function timer8_.* \
    --allowlist-function timer16_.* \
    --allowlist-function usart_.* \
    --allowlist-function exti_.* \
    --allowlist-function tcn75_.* \
    --allowlist-type EXTI_Ctrl_Device \
    --allowlist-type PCI_Ctrl_Device \
    --allowlist-type SPI_Device \
    --allowlist-type timer2_prescaler_t \
    --allowlist-type timer_prescaler_t \
    --allowlist-type timer16_interrupt_t \
    --allowlist-type z_callsaved_ctx \
    --allowlist-type z_callused_ctx \
    --allowlist-type z_intctx \
    --allowlist-type K_* \
    --allowlist-type Z_* \
    -o \
    rust-avrtos-sys/src/bindings.rs \
    -- \
    -I /usr/avr/include \
    -I src \
    -I src/avrtos \
    --target=avr \
    -mmcu=atmega2560 \
    -Os \
    -D__AVR_3_BYTE_PC__ \
    -DF_CPU=16000000UL \
    -DCONFIG_RUST=1

# todo evaluate:
# --translate-enum-integer-types