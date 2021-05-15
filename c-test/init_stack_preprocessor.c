#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)
struct base_stack_s
{
    uint8_t sreg;
    uint8_t r0r23[24u];
    uint16_t context;
    uint8_t r26r31[6u];
    uint16_t ret_addr; // inverted
};

struct stack_s
{
    uint8_t empty[0x100u - 35u];
    struct base_stack_s base;
};

struct stack_s stack0 = {
    .empty = {0x00},
    .base = {.sreg = 0x02,
             .r0r23 = 0x00,
             .context = 0xABCD,
             .r26r31 = 0x00,
             .ret_addr = 0x2301}};

struct
{
    uint8_t empty[0x100u - 35u];
    struct
    {
        uint8_t sreg;
        uint8_t r0r23[24u];
        uint16_t context;
        uint8_t r26r31[6u];
        uint16_t ret_addr;
    } base;
} stack1 = {
    .empty = {0x00},
    .base = {.sreg = 0x02,
             .r0r23 = 0x00,
             .context = 0xABCD,
             .r26r31 = 0x00,
             .ret_addr = 0x2301}};

#define _K_STACK_INITIALIZER(name, stack_size, context_p, entry) \
    struct stack_s _k_stack_buf_##name = {                       \
        .empty = {0x00},                                         \
        .base = {                                                \
            .sreg = 0x02,                                        \
            .r0r23 = 0x00,                                       \
            .context = (uint16_t)context_p,                      \
            .r26r31 = 0x00,                                      \
            .ret_addr = (uint16_t)entry}}

#pragma pack(pop)

_K_STACK_INITIALIZER(my, 0x100, 0xABCD, 0x2301);

int main(void)
{
    printf("stack size      = %lu\n", sizeof(_k_stack_buf_my));
    printf("base stack size = %lu\n", sizeof(struct base_stack_s));

    for (uint16_t i = 0; i < 0x100; i++)
    {
        if (i % 16 == 0)
        {
            printf("\n");
        }
        else
        {
            printf(" ");
        }

        printf("%02X", ((uint8_t *)&_k_stack_buf_my)[i]);
    }
}