
.pio/build/pro16MHzatmega328/firmware.elf:     file format elf32-avr


Disassembly of section .text:

00000000 <__vectors>:
   0:	0c 94 34 00 	jmp	0x68	; 0x68 <__ctors_end>
   4:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
   8:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
   c:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  10:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  14:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  18:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  1c:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  20:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  24:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  28:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  2c:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  30:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  34:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  38:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  3c:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  40:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  44:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  48:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  4c:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  50:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  54:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  58:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  5c:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  60:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>
  64:	0c 94 49 00 	jmp	0x92	; 0x92 <__bad_interrupt>

00000068 <__ctors_end>:
  68:	11 24       	eor	r1, r1
  6a:	1f be       	out	0x3f, r1	; 63
  6c:	cf ef       	ldi	r28, 0xFF	; 255
  6e:	d8 e0       	ldi	r29, 0x08	; 8
  70:	de bf       	out	0x3e, r29	; 62
  72:	cd bf       	out	0x3d, r28	; 61

00000074 <__do_copy_data>:
  74:	11 e0       	ldi	r17, 0x01	; 1
  76:	a0 e0       	ldi	r26, 0x00	; 0
  78:	b1 e0       	ldi	r27, 0x01	; 1
  7a:	e6 e3       	ldi	r30, 0x36	; 54
  7c:	f2 e0       	ldi	r31, 0x02	; 2
  7e:	02 c0       	rjmp	.+4      	; 0x84 <__do_copy_data+0x10>
  80:	05 90       	lpm	r0, Z+
  82:	0d 92       	st	X+, r0
  84:	aa 32       	cpi	r26, 0x2A	; 42
  86:	b1 07       	cpc	r27, r17
  88:	d9 f7       	brne	.-10     	; 0x80 <__do_copy_data+0xc>
  8a:	0e 94 c5 00 	call	0x18a	; 0x18a <main>
  8e:	0c 94 19 01 	jmp	0x232	; 0x232 <_exit>

00000092 <__bad_interrupt>:
  92:	0c 94 00 00 	jmp	0	; 0x0 <__vectors>

00000096 <led_init>:
  96:	0f 93       	push	r16
  98:	00 e2       	ldi	r16, 0x20	; 32
  9a:	00 93 24 00 	sts	0x0024, r16	; 0x800024 <__TEXT_REGION_LENGTH__+0x7e0024>
  9e:	0f 91       	pop	r16
  a0:	08 95       	ret

000000a2 <led_on>:
  a2:	0f 93       	push	r16
  a4:	00 e2       	ldi	r16, 0x20	; 32
  a6:	00 93 25 00 	sts	0x0025, r16	; 0x800025 <__TEXT_REGION_LENGTH__+0x7e0025>
  aa:	0f 91       	pop	r16
  ac:	08 95       	ret

000000ae <led_off>:
  ae:	0f 93       	push	r16
  b0:	00 e0       	ldi	r16, 0x00	; 0
  b2:	00 93 25 00 	sts	0x0025, r16	; 0x800025 <__TEXT_REGION_LENGTH__+0x7e0025>
  b6:	0f 91       	pop	r16
  b8:	08 95       	ret

000000ba <_Z14usart_transmitc>:
  ba:	90 91 c0 00 	lds	r25, 0x00C0	; 0x8000c0 <__TEXT_REGION_LENGTH__+0x7e00c0>
  be:	95 ff       	sbrs	r25, 5
  c0:	fc cf       	rjmp	.-8      	; 0xba <_Z14usart_transmitc>
  c2:	80 93 c6 00 	sts	0x00C6, r24	; 0x8000c6 <__TEXT_REGION_LENGTH__+0x7e00c6>
  c6:	08 95       	ret

000000c8 <_Z9usart_hexh>:
  c8:	cf 93       	push	r28
  ca:	e8 2f       	mov	r30, r24
  cc:	ef 70       	andi	r30, 0x0F	; 15
  ce:	f0 e0       	ldi	r31, 0x00	; 0
  d0:	ee 5e       	subi	r30, 0xEE	; 238
  d2:	fe 4f       	sbci	r31, 0xFE	; 254
  d4:	c0 81       	ld	r28, Z
  d6:	e8 2f       	mov	r30, r24
  d8:	f0 e0       	ldi	r31, 0x00	; 0
  da:	84 e0       	ldi	r24, 0x04	; 4
  dc:	f5 95       	asr	r31
  de:	e7 95       	ror	r30
  e0:	8a 95       	dec	r24
  e2:	e1 f7       	brne	.-8      	; 0xdc <_Z9usart_hexh+0x14>
  e4:	ee 5e       	subi	r30, 0xEE	; 238
  e6:	fe 4f       	sbci	r31, 0xFE	; 254
  e8:	80 81       	ld	r24, Z
  ea:	0e 94 5d 00 	call	0xba	; 0xba <_Z14usart_transmitc>
  ee:	8c 2f       	mov	r24, r28
  f0:	cf 91       	pop	r28
  f2:	0c 94 5d 00 	jmp	0xba	; 0xba <_Z14usart_transmitc>

000000f6 <_Z11usart_hex16j>:
  f6:	cf 93       	push	r28
  f8:	c8 2f       	mov	r28, r24
  fa:	89 2f       	mov	r24, r25
  fc:	0e 94 64 00 	call	0xc8	; 0xc8 <_Z9usart_hexh>
 100:	8c 2f       	mov	r24, r28
 102:	cf 91       	pop	r28
 104:	0c 94 64 00 	jmp	0xc8	; 0xc8 <_Z9usart_hexh>

00000108 <_Z10usart_sendPKcj>:
 108:	ef 92       	push	r14
 10a:	ff 92       	push	r15
 10c:	0f 93       	push	r16
 10e:	1f 93       	push	r17
 110:	cf 93       	push	r28
 112:	8c 01       	movw	r16, r24
 114:	7b 01       	movw	r14, r22
 116:	c0 e0       	ldi	r28, 0x00	; 0
 118:	8c 2f       	mov	r24, r28
 11a:	90 e0       	ldi	r25, 0x00	; 0
 11c:	8e 15       	cp	r24, r14
 11e:	9f 05       	cpc	r25, r15
 120:	40 f4       	brcc	.+16     	; 0x132 <_Z10usart_sendPKcj+0x2a>
 122:	80 0f       	add	r24, r16
 124:	91 1f       	adc	r25, r17
 126:	fc 01       	movw	r30, r24
 128:	80 81       	ld	r24, Z
 12a:	0e 94 5d 00 	call	0xba	; 0xba <_Z14usart_transmitc>
 12e:	cf 5f       	subi	r28, 0xFF	; 255
 130:	f3 cf       	rjmp	.-26     	; 0x118 <_Z10usart_sendPKcj+0x10>
 132:	cf 91       	pop	r28
 134:	1f 91       	pop	r17
 136:	0f 91       	pop	r16
 138:	ff 90       	pop	r15
 13a:	ef 90       	pop	r14
 13c:	08 95       	ret

0000013e <usart_show_addr>:
 13e:	cf 93       	push	r28
 140:	df 93       	push	r29
 142:	ec 01       	movw	r28, r24
 144:	67 e0       	ldi	r22, 0x07	; 7
 146:	70 e0       	ldi	r23, 0x00	; 0
 148:	82 e2       	ldi	r24, 0x22	; 34
 14a:	91 e0       	ldi	r25, 0x01	; 1
 14c:	0e 94 84 00 	call	0x108	; 0x108 <_Z10usart_sendPKcj>
 150:	ce 01       	movw	r24, r28
 152:	0e 94 7b 00 	call	0xf6	; 0xf6 <_Z11usart_hex16j>
 156:	8a e0       	ldi	r24, 0x0A	; 10
 158:	df 91       	pop	r29
 15a:	cf 91       	pop	r28
 15c:	0c 94 5d 00 	jmp	0xba	; 0xba <_Z14usart_transmitc>

00000160 <return_sp_assembler>:
 160:	80 91 5d 00 	lds	r24, 0x005D	; 0x80005d <__TEXT_REGION_LENGTH__+0x7e005d>
 164:	90 91 5e 00 	lds	r25, 0x005E	; 0x80005e <__TEXT_REGION_LENGTH__+0x7e005e>
 168:	08 95       	ret

0000016a <push_things_in_stack>:
 16a:	8b eb       	ldi	r24, 0xBB	; 187
 16c:	9b eb       	ldi	r25, 0xBB	; 187
 16e:	08 95       	ret

00000170 <read_return_addr>:
 170:	af 93       	push	r26
 172:	bf 93       	push	r27
 174:	ad e5       	ldi	r26, 0x5D	; 93
 176:	be e5       	ldi	r27, 0x5E	; 94
 178:	8d 91       	ld	r24, X+
 17a:	8d 91       	ld	r24, X+
 17c:	8d 91       	ld	r24, X+
 17e:	9d 91       	ld	r25, X+
 180:	0e 94 9f 00 	call	0x13e	; 0x13e <usart_show_addr>
 184:	bf 91       	pop	r27
 186:	af 91       	pop	r26
 188:	08 95       	ret

0000018a <main>:
 18a:	0e 94 4b 00 	call	0x96	; 0x96 <led_init>
 18e:	10 92 c5 00 	sts	0x00C5, r1	; 0x8000c5 <__TEXT_REGION_LENGTH__+0x7e00c5>
 192:	87 e6       	ldi	r24, 0x67	; 103
 194:	80 93 c4 00 	sts	0x00C4, r24	; 0x8000c4 <__TEXT_REGION_LENGTH__+0x7e00c4>
 198:	88 e1       	ldi	r24, 0x18	; 24
 19a:	80 93 c1 00 	sts	0x00C1, r24	; 0x8000c1 <__TEXT_REGION_LENGTH__+0x7e00c1>
 19e:	86 e0       	ldi	r24, 0x06	; 6
 1a0:	80 93 c2 00 	sts	0x00C2, r24	; 0x8000c2 <__TEXT_REGION_LENGTH__+0x7e00c2>
 1a4:	0e 94 b8 00 	call	0x170	; 0x170 <read_return_addr>
 1a8:	0e 94 7b 00 	call	0xf6	; 0xf6 <_Z11usart_hex16j>
 1ac:	8a e0       	ldi	r24, 0x0A	; 10
 1ae:	0e 94 5d 00 	call	0xba	; 0xba <_Z14usart_transmitc>
 1b2:	0d b7       	in	r16, 0x3d	; 61
 1b4:	1e b7       	in	r17, 0x3e	; 62
 1b6:	e8 01       	movw	r28, r16
 1b8:	22 97       	sbiw	r28, 0x02	; 2
 1ba:	6d e0       	ldi	r22, 0x0D	; 13
 1bc:	70 e0       	ldi	r23, 0x00	; 0
 1be:	84 e0       	ldi	r24, 0x04	; 4
 1c0:	91 e0       	ldi	r25, 0x01	; 1
 1c2:	0e 94 84 00 	call	0x108	; 0x108 <_Z10usart_sendPKcj>
 1c6:	08 5f       	subi	r16, 0xF8	; 248
 1c8:	1f 4f       	sbci	r17, 0xFF	; 255
 1ca:	ce 01       	movw	r24, r28
 1cc:	0e 94 7b 00 	call	0xf6	; 0xf6 <_Z11usart_hex16j>
 1d0:	63 e0       	ldi	r22, 0x03	; 3
 1d2:	70 e0       	ldi	r23, 0x00	; 0
 1d4:	80 e0       	ldi	r24, 0x00	; 0
 1d6:	91 e0       	ldi	r25, 0x01	; 1
 1d8:	0e 94 84 00 	call	0x108	; 0x108 <_Z10usart_sendPKcj>
 1dc:	88 81       	ld	r24, Y
 1de:	0e 94 64 00 	call	0xc8	; 0xc8 <_Z9usart_hexh>
 1e2:	8a e0       	ldi	r24, 0x0A	; 10
 1e4:	0e 94 5d 00 	call	0xba	; 0xba <_Z14usart_transmitc>
 1e8:	cf 3f       	cpi	r28, 0xFF	; 255
 1ea:	28 e0       	ldi	r18, 0x08	; 8
 1ec:	d2 07       	cpc	r29, r18
 1ee:	21 f0       	breq	.+8      	; 0x1f8 <main+0x6e>
 1f0:	21 96       	adiw	r28, 0x01	; 1
 1f2:	0c 17       	cp	r16, r28
 1f4:	1d 07       	cpc	r17, r29
 1f6:	49 f7       	brne	.-46     	; 0x1ca <main+0x40>
 1f8:	6d e0       	ldi	r22, 0x0D	; 13
 1fa:	70 e0       	ldi	r23, 0x00	; 0
 1fc:	84 e0       	ldi	r24, 0x04	; 4
 1fe:	91 e0       	ldi	r25, 0x01	; 1
 200:	0e 94 84 00 	call	0x108	; 0x108 <_Z10usart_sendPKcj>
 204:	0e 94 51 00 	call	0xa2	; 0xa2 <led_on>
 208:	8f ef       	ldi	r24, 0xFF	; 255
 20a:	99 e6       	ldi	r25, 0x69	; 105
 20c:	28 e1       	ldi	r18, 0x18	; 24
 20e:	81 50       	subi	r24, 0x01	; 1
 210:	90 40       	sbci	r25, 0x00	; 0
 212:	20 40       	sbci	r18, 0x00	; 0
 214:	e1 f7       	brne	.-8      	; 0x20e <main+0x84>
 216:	00 c0       	rjmp	.+0      	; 0x218 <main+0x8e>
 218:	00 00       	nop
 21a:	0e 94 57 00 	call	0xae	; 0xae <led_off>
 21e:	8f ef       	ldi	r24, 0xFF	; 255
 220:	99 e6       	ldi	r25, 0x69	; 105
 222:	28 e1       	ldi	r18, 0x18	; 24
 224:	81 50       	subi	r24, 0x01	; 1
 226:	90 40       	sbci	r25, 0x00	; 0
 228:	20 40       	sbci	r18, 0x00	; 0
 22a:	e1 f7       	brne	.-8      	; 0x224 <main+0x9a>
 22c:	00 c0       	rjmp	.+0      	; 0x22e <main+0xa4>
 22e:	00 00       	nop
 230:	e9 cf       	rjmp	.-46     	; 0x204 <main+0x7a>

00000232 <_exit>:
 232:	f8 94       	cli

00000234 <__stop_program>:
 234:	ff cf       	rjmp	.-2      	; 0x234 <__stop_program>
