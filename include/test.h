#ifndef _F_H
#define _F_H

#include <avr/io.h>

#include <string.h>
#include <stdio.h>

#include "uart.h"
#include "utils.h"

/*___________________________________________________________________________*/

void testfunction(void); 

/*___________________________________________________________________________*/

extern "C" uint16_t read_sp(void);
extern "C" uint16_t read_ra(void);

/*___________________________________________________________________________*/

extern "C" uint16_t read_return_addr();
extern "C" uint16_t push_things_in_stack(void);

/*___________________________________________________________________________*/

#endif 