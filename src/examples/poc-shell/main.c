#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <avrtos/misc/uart.h>
#include <avrtos/misc/led.h>

#include <avrtos/kernel.h>
#include <avrtos/debug.h>

/*___________________________________________________________________________*/

#define K_MODULE K_MODULE_APPLICATION

/*___________________________________________________________________________*/

void consumer(void *context);

K_THREAD_DEFINE(w1, consumer, 0x100, K_PREEMPTIVE, NULL, 'A');

struct in
{
        struct qitem tie;
        char buffer[20];
        uint8_t len;
};

K_MEM_SLAB_DEFINE(myslab, sizeof(struct in), 2u);
K_FIFO_DEFINE(myfifo);

void push(struct in **mem)
{
        k_fifo_put(&myfifo, *(void **)mem);
        *mem = NULL;
}

int8_t alloc(struct in **mem)
{
        return k_mem_slab_alloc(&myslab, (void **)mem, K_NO_WAIT);
}

void free(struct in *mem)
{
        k_mem_slab_free(&myslab, mem);
}

/*___________________________________________________________________________*/

static inline void input(const char rx)
{
        static struct in *mem = NULL;
        if (mem == NULL) {
                if (alloc(&mem) != 0) {
                        __ASSERT_NULL(mem);
                        usart_transmit('!');
                        return;
                }
                mem->len = 0;
        }

        switch (rx) {
        case 0x1A: /* Ctrl + Z -> drop */
                mem->len = 0;
        case '\n': /* process the packet */
                mem->buffer[mem->len] = '\0';
                push(&mem);
                break;
        case 0x08: /* backspace */
                if (mem->len > 0) {
                        mem->len--;
                        usart_transmit(rx);
                }
                break;
        default:
                if (mem->len == sizeof(mem->buffer) - 1u) {
                        mem->len = 0;
                        push(&mem);
                } else {
                        mem->buffer[mem->len++] = rx;
                }
                usart_transmit(rx);
                break;
        }
}

ISR(USART_RX_vect)
{
        const char rx = UDR0;
        input(rx);
}

void consumer(void *context)
{
        for (;;) {
                usart_print("\n# ");
                struct in *mem = (struct in *)k_fifo_get(&myfifo, K_FOREVER);
                __ASSERT_NOTNULL(mem);
                if (mem->len == 0) {
                        usart_print("\nCOMMAND DROPPED !");
                } else {
                        /* process/parsed the command */
                        usart_print("CMD received ! len = ");
                        usart_u8(mem->len);
                }
                k_mem_slab_free(&myslab, mem);
        }
}

int main(void)
{
        usart_init();

        k_thread_dump_all();

        SET_BIT(UCSR0B, 1 << RXCIE0);

        k_sleep(K_FOREVER);
}

/*___________________________________________________________________________*/