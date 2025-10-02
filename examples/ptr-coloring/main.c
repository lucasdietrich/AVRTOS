/*
 * Copyright (c) 2025 Lucas Dietrich <lucas.dietrich.git@proton.me>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file main.c
 * @brief Pointer coloring demonstration example.
 *
 * This example demonstrates the pointer coloring functionality in AVRTOS.
 * Pointer coloring allows embedding small color values (0-3) in the unused
 * bits of AVR pointers, which are 16-bit but only address 14-bit of SRAM.
 *
 * The example shows how to:
 * - Colorize pointers with different colors
 * - Decolorize pointers to get the original address
 * - Extract color information from colored pointers
 *
 * This can be useful for debugging, tagging pointers, or implementing
 * colored pointer algorithms.
 */

#include <stdio.h>

#include <avrtos/avrtos.h>

#include "avrtos/sys.h"
#include "serial.h"

static int var1 = 42;

/**
 * @brief Display pointer information including color.
 *
 * This function prints the original pointer, its color value, and the
 * decolorized (original) pointer address.
 *
 * @param ptrs Array of pointers to display
 * @param len Number of pointers in the array
 */
void display_ptrs(void **ptrs, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        void *ptr             = ptrs[i];
        void *decolorized_ptr = sys_ptr_decolorize(ptr);
        uint8_t color         = sys_ptr_color(ptr);
        printf_P(PSTR("ptr: %p | color: %u | decolorized: %p\n"), ptr, color,
                 decolorized_ptr);
    }
}

int main(void)
{
    int var2 = 0;

    /* Start with original pointers */
    void *ptrs[] = {&var1, &var2};
    printf_P(PSTR("Original pointers:\n"));
    display_ptrs(ptrs, ARRAY_SIZE(ptrs));

    /* Colorize with blue */
    for (uint8_t i = 0; i < ARRAY_SIZE(ptrs); i++) {
        void *decolored = sys_ptr_decolorize(ptrs[i]); /* Ensure decolorized first */
        ptrs[i]         = sys_ptr_colorize(decolored, SYS_PTR_COLOR_BLUE);
    }
    printf_P(PSTR("\nAfter coloring with BLUE:\n"));
    display_ptrs(ptrs, ARRAY_SIZE(ptrs));

    /* Change to red */
    for (uint8_t i = 0; i < ARRAY_SIZE(ptrs); i++) {
        void *decolored = sys_ptr_decolorize(ptrs[i]);
        ptrs[i]         = sys_ptr_colorize(decolored, SYS_PTR_COLOR_RED);
    }
    printf_P(PSTR("\nAfter coloring with RED:\n"));
    display_ptrs(ptrs, ARRAY_SIZE(ptrs));

    /* Change to green */
    for (uint8_t i = 0; i < ARRAY_SIZE(ptrs); i++) {
        void *decolored = sys_ptr_decolorize(ptrs[i]);
        ptrs[i]         = sys_ptr_colorize(decolored, SYS_PTR_COLOR_GREEN);
    }
    printf_P(PSTR("\nAfter coloring with GREEN:\n"));
    display_ptrs(ptrs, ARRAY_SIZE(ptrs));
}