#include "debug.h"

/*___________________________________________________________________________*/

void testfunction()
{
  uint16_t ra = read_ra();

  char buffer[16 * 8];
  memset(buffer, 0xAA, sizeof(buffer));

  usart_ram_dump(RAMSTART, RAMEND - RAMSTART + 1, SP);

  usart_show_addr(ra);
}

/*___________________________________________________________________________*/

void show_stack_pointer(struct thread_t *th)
{
  usart_hex16((uint16_t) th->sp);
}

