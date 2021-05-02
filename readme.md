# Multithreading

Index:
- References to documentation and help
- Program analysis [tag = test] : in order to understand how `SP`, `REG` and *return address* are handled during function calls
- Cooperative multithreading [tag = coop] : 
- Commands list

Themes:
1. 2 cooperative threads
2. 2 preemtive threads

Tags:
- test
- coop

---

## References : 

### Return addres:
![return_address.png](./pics/return_address.png)

### Data memory map

- Stack pointer : 0x8FF for 2048B RAM size

- From *iom328p.h*
```cpp
#define RAMSTART     (0x100)
#define RAMEND       0x8FF     /* Last On-Chip SRAM Location */
```

- *common.h* file shows:
```cpp
#define SPL _SFR_IO8(0x3D)
#define SPH _SFR_IO8(0x3E)
#define SREG _SFR_IO8(0x3F)
```

With `#define __SFR_OFFSET 0x20`.

![data_memory_map.png](./pics/data_memory_map.png)

### Stack pointer 

![stack_pointer.png](./pics/stack_pointer.png)

### Architecture

![harvard_datasheet.png](./pics/harvard_datasheet.png)

---

## Program analysis

See :
- [res/disassembly.test.s](./res/disassembly.test.s)
- [res/ramdump.test.parsed.txt](.res/ramdump.test.parsed.txt)
- [res/ramdump.test.txt](./res/ramdump.test.txt)

### Decription

With this function :
- we test the *return address* of the call of the *read_ra* function
- we defines a buffer of 128 bytes (canaries with the values *0xAA*) in the stack.
- we dump the RAM
- we show the previous *return address*

```cpp
void testfunction()
{
  uint16_t ra = read_ra();

  char buffer[16 * 8];
  memset(buffer, 0xAA, sizeof(buffer));

  usart_ram_dump(RAMSTART, RAMEND - RAMSTART + 1, SP);

  usart_show_addr(ra);
}
```

This test function address is stored in the variable `function_p` like this:
- We do this this way in order to provent the compiler to optimize the function.
- We would have been used the `__attribute__((optimize(0)))` (not tested)

```cpp
void(*function_p)(void) = testfunction;
```

This function is called in the main function :

```cpp
int main(void)
{
  led_init();
  usart_init();

  //////////////////////////////////////////////

  function_p();

  usart_show_addr((uint16_t) main << 1);        // fixed addr
  usart_show_addr((uint16_t) function_p << 1);  // fixed addr

  //////////////////////////////////////////////

  while(1)
  {
    led_on();

    _delay_ms(500.0);

    led_off();
    
    _delay_ms(500.0);
  }
}
```

The *return address* is stored onto the stack (see table 7-1. above), the `CALL` instruction increment the *stack pointer* of 2 by pushing the *return address* onto the stack.

The following function (`read_ra`) gets the *return address* of the *callee* after a `call` to this function and stores it in `ra`

```asm
// return the return address of when ther callee calls it
// return the return address of when ther callee calls it
read_ra:
    pop r25
    pop r24

    push r24
    push r25

    // shift the address (addr_real = addr_cpu << 1)
    add r24, r24
    adc r25, r25

    ret
```

The *return address* refers to an instruction and considering that instructions are aligned on paired addresses, the AVR architecture imposes to use addresse shifted by 1bit to the right (i.e. divided by 2). e.g. the *return address* = 0x00A2 will be stored as 0x0051 (0x00A2/2) in stack.

This is also related to harvard architecture that seperate addressing for program and ram, which is not the case for Von Neumann architectures.

### Disassembly

***main (not complete)**
```asm
0000024c <main>:
 24c:	0e 94 4b 00 	call	0x96	; 0x96 <led_init>
 250:	10 92 c5 00 	sts	0x00C5, r1	; 0x8000c5 <__TEXT_REGION_LENGTH__+0x7e00c5>
 254:	88 e0       	ldi	r24, 0x08	; 8
 256:	80 93 c4 00 	sts	0x00C4, r24	; 0x8000c4 <__TEXT_REGION_LENGTH__+0x7e00c4>
 25a:	88 e1       	ldi	r24, 0x18	; 24
 25c:	80 93 c1 00 	sts	0x00C1, r24	; 0x8000c1 <__TEXT_REGION_LENGTH__+0x7e00c1>
 260:	86 e0       	ldi	r24, 0x06	; 6
 262:	80 93 c2 00 	sts	0x00C2, r24	; 0x8000c2 <__TEXT_REGION_LENGTH__+0x7e00c2>
 266:	0e 94 aa 00 	call	0x154	; 0x154 <_Z12testfunctionv>
 26a:	86 e2       	ldi	r24, 0x26	; 38
 26c:	91 e0       	ldi	r25, 0x01	; 1
 26e:	88 0f       	add	r24, r24
 270:	99 1f       	adc	r25, r25
 272:	0e 94 96 00 	call	0x12c	; 0x12c <usart_show_addr>
 276:	8a ea       	ldi	r24, 0xAA	; 170
 278:	90 e0       	ldi	r25, 0x00	; 0
 27a:	88 0f       	add	r24, r24
 27c:	99 1f       	adc	r25, r25
 27e:	0e 94 96 00 	call	0x12c	; 0x12c <usart_show_addr>

 ... MORE ...
```

**testfunction (not complete)**

```
00000154 <_Z12testfunctionv>:
 154:	cf 92       	push	r12
 156:	df 92       	push	r13
 158:	ef 92       	push	r14
 15a:	ff 92       	push	r15
 15c:	0f 93       	push	r16
 15e:	1f 93       	push	r17
 160:	cf 93       	push	r28
 162:	df 93       	push	r29
 164:	cd b7       	in	r28, 0x3d	; 61
 166:	de b7       	in	r29, 0x3e	; 62
 168:	c0 58       	subi	r28, 0x80	; 128
 16a:	d1 09       	sbc	r29, r1
 16c:	0f b6       	in	r0, 0x3f	; 63
 16e:	f8 94       	cli
 170:	de bf       	out	0x3e, r29	; 62
 172:	0f be       	out	0x3f, r0	; 63
 174:	cd bf       	out	0x3d, r28	; 61
 176:	0e 94 0a 01 	call	0x214	; 0x214 <read_ra>
 17a:	7c 01       	movw	r14, r24
 17c:	40 e8       	ldi	r20, 0x80	; 128
 17e:	50 e0       	ldi	r21, 0x00	; 0
 180:	6a ea       	ldi	r22, 0xAA	; 170
 182:	70 e0       	ldi	r23, 0x00	; 0
 184:	ce 01       	movw	r24, r28
 186:	01 96       	adiw	r24, 0x01	; 1
 188:	0e 94 58 01 	call	0x2b0	; 0x2b0 <memset>
 18c:	cd b6       	in	r12, 0x3d	; 61
 18e:	de b6       	in	r13, 0x3e	; 62
 190:	6d e0       	ldi	r22, 0x0D	; 13

 ... MORE ...
 ```

### RAMDUMP

*ramdump* during the `testfunction` call.

```
0100 > 010F : 20 20 3C 2D 2D 20 68 65 72 65 00 20 3A 20 00 3D    <-- here. : .=
0110 > 011F : 3D 3D 3D 3D 3D 3D 3D 3D 3D 3D 3D 0A 00 30 31 32  ===========..012
0120 > 012F : 33 34 35 36 37 38 39 41 42 43 44 45 46 61 64 64  3456789ABCDEFadd
0130 > 013F : 72 20 3D 20 00 00 00 00 00 00 00 00 00 00 00 00  r = ............
0140 > 014F : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
...
...
...
0850 > 085F : 00 C0 FF 80 00 C0 FF 80 00 C0 FF 80 00 C0 FF 80  ................
0860 > 086F : 00 C0 FF 80 00 C0 FF 80 00 C0 FF 00 8E 73 08 6F  .............s.o
0870 > 087F : 01 7A 00 DC AA AA AA AA AA AA AA AA AA AA AA AA  .z..............
0880 > 088F : AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA  ................
0890 > 089F : AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA  ................
08A0 > 08AF : AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA  ................
08B0 > 08BF : AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA  ................
08C0 > 08CF : AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA  ................
08D0 > 08DF : AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA  ................
08E0 > 08EF : AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA  ................
08F0 > 08FF : AA AA AA AA 08 FF 01 0F 42 41 95 0F 01 35 00 47  ........BA...5.G
```

### Analysis

Interesting part is at the end :
- in red : the buffer of 128bytes of `0xAA`
- in green : save of the 8 used registers : r12 > r17, r28, r29
- in blue : the *return addr* after the call of the `testfunction` :
    - address is shifted by 1 to the right : to restore it `0x0135 << 1 = 0x26A` which is exactly the line after the `call testfunctionv` in `main` function
    - See `main` disassembly : `266:	0e 94 aa 00 	call	0x154	; 0x154 <_Z12testfunctionv>`
- in violet : *return addr* after the call of the `main` :
    - address is shifted by 1 to the right : to restore it `0x0047 << 1 = 0x008e` which is exactly the line after the `call main` in `__do_copy_data` (function which calls main)

![testfunction_ram.png](./pics/testfunction_ram.png)

### Modifying the stack pointer
In order to modify manyally the stack pointer the following procedure is proceed :

The stack pointer is manually shifted to `+128bytes` in order to store the `buffer` in stack without the use of a lot of `push`.

Disassembly is :
- r28, r29 contains the SP value to be written

```asm
 11c:	0f b6       	in	r0, 0x3f	; save SREG flags
 11e:	f8 94       	cli             ; disable interrupts (immediate)
 120:	de bf       	out	0x3e, r29	; write SPH
 122:	0f be       	out	0x3f, r0	; restore SREG flags (and interrupts if sei)
 124:	cd bf       	out	0x3d, r28	; write SPL
```

Datasheet says:
- `cli` : The interrupts will be immediately disabled.
- `sei` : The instruction following SEI will be executed before any pending interrupts.
    - this explains that restoring the SREG flags is not the last instruction by the `n-1`

#### Stack pointer initialization
Initialization of the stack pointer is done after reset, here :

```asm
00000068 <__ctors_end>:
  68:	11 24       	eor	r1, r1    ; ldi r1, 0 (equivalent)
  6a:	1f be       	out	0x3f, r1	; SREG = 0
  6c:	cf ef       	ldi	r28, 0xFF	; prepare SPL value
  6e:	d8 e0       	ldi	r29, 0x08	; prepare SPH value
  70:	de bf       	out	0x3e, r29	; write SPH
  72:	cd bf       	out	0x3d, r28	; write SPL
```

---

## Cooperative multithreading


See :
- [res/disassembly.coop.s](./res/disassembly.coop.s)
- [res/ramdump.coop.parsed.txt](.res/ramdump.coop.parsed.txt)
- [res/ramdump.coop.txt](./res/ramdump.coop.txt)

**Description**

complete cooperative multithreading for 2 threads

Output : 
```
============
thread 2 address : 00A4
thread 2 stack context : 0100
thread 2 stack pointer address : 04DC
thread 2 stack base address : 04FF
#1 loop : 08FD
EF
CD AB 89 #2 loop SP = 04FF
#1 loop : 08FD
#2 loop SP = 04FF
#1 loop : 08FD
#2 loop SP = 04FF
#1 loop : 08FD
#2 loop SP = 04FF
#1 loop : 08FD
#2 loop SP = 04FF
#1 loop : 08FD
#2 loop SP = 04FF
#1 loop : 08FD
#2 loop SP = 04FF
#1 loop : 08FD
#2 loop SP = 04FF
#1 loop : 08FD
#2 loop SP = 04FF
```

---

## Commands

### Disassembly

Go to project directory

`cd /mnt/c/Users/ldade/Documents/ProjetsRecherche/Embedded/ATmega328p-multithreading`

Disassembly

`avr-objdump -S .pio/build/pro16MHzatmega328/firmware.elf > disassembly.s`
