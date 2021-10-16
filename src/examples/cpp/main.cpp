#include <avrtos/kernel.h>
#include <avrtos/debug.h>

int main(void)
{
    usart_init();
    k_thread_dump_all();

    k_sleep(K_FOREVER);
}