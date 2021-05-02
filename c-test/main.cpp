// on WSL 2 (Ubuntu 20.04 LTS)
// compile with command:
//      gcc main.cpp -o main
// exectute with
//      ./main

#include <stdio.h>
#include <stdint.h>

struct thread {
    uint16_t sp;
};

int main(void)
{
    struct thread threads[2];

    threads[0].sp = 0xABCD;

    ((uint8_t*) &threads[1].sp)[0] = 0xAB;

    ((uint8_t*) &threads[1].sp)[1] = 0xCD;

    printf("%x\n", threads[0].sp);
    printf("%x\n", threads[1].sp);
}
