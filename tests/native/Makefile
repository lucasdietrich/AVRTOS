#
# Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
#
# SPDX-License-Identifier: Apache-2.0
#
# Native (host) unit tests for portable AVRTOS data structures.
# No avr-gcc/QEMU required: sources are compiled as-is against a tiny
# avr/io.h stub, since dstruct code doesn't touch real AVR registers.

CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -g -D__AVR_2_BYTE_PC__ -DCONFIG_KERNEL_ARGS_CHECKS=1
SRC_DIR := ../../src/avrtos
STUB_DIR := avr_stub

BIN := test_tqueue

$(BIN): test_tqueue.c $(SRC_DIR)/dstruct/tqueue.c
	$(CC) $(CFLAGS) -I$(STUB_DIR) -iquote$(SRC_DIR) -iquote$(SRC_DIR)/.. $^ -o $@

.PHONY: run clean
run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)
