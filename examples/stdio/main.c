/*
 * Copyright (c) 2024 Lucas Dietrich <ld.adecy@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <avrtos/avrtos.h>

int main(void)
{
    printf("Hello, World!\n");

    for (;;) {
        // read a character from the standard input
        int c = getchar();
        if (c > 0) {
            // echo the character back to the standard output
            putchar(c);
        }
    }

    return 0;
}