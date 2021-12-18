#ifndef _AVRTOS_FAULT_H_
#define _AVRTOS_FAULT_H_

#include <stdint.h>

#include "avrtos.h"

#ifdef __cplusplus
extern "C" {
#endif

#define K_FAULT_ASSERT    	0
#define K_FAULT_SENTINEL	1


void __fault(uint8_t reason);

#ifdef __cplusplus
}
#endif

#endif /* _AVRTOS_FAULT_H_ */