# Multithreading

2 threads

thread_switch()

---

## Reference : 

### Return addres:
![return_address.png](./pics/return_address.png)

### Data memory map

- Stack pointer : 0x8FF for 2048B RAM size

- From *iom328p.h*
```cpp
#define RAMSTART     (0x100)
#define RAMEND       0x8FF     /* Last On-Chip SRAM Location */
```

- *common.h* fle shows:
```cpp
#define SPL _SFR_IO8(0x3D)
#define SPH _SFR_IO8(0x3E)
```

With `#define __SFR_OFFSET 0x20`.

![data_memory_map.png](./pics/data_memory_map.png)

### Stack pointer 

![stack_pointer.png](./pics/stack_pointer.png)

---

## Disassembly

Go to project directory

`cd /mnt/c/Users/ldade/Documents/ProjetsRecherche/Embedded/ATmega328p-multithreading`

Disassembly

`avr-objdump -S .pio/build/pro16MHzatmega328/firmware.elf > diassembly.s`