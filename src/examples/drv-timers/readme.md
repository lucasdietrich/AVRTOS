# Timers Driver Sample

This sample demonstrates how to use the timers driver with 8bits and 16bits timers.

- Tested with ATmega2560
- Should work with ATmega328P
- Emulation not supported on QEMU (because of the use of several hardware timers)

Commands :
- Build `make -C build upload_sample_drv_timers`
- Upload `make -C build upload_sample_drv_timers`
- Monitor `make monitor`

Once compiled you will find the elf, binary, hex and disassembly files in : `build/src/examples/drv-timers/`

Note:
Use provided macros to minimize code generated, only a small overhead is introduced (+ 2 instructions). e.g. `TIMER4_OVF_vect` IRQ handler which generated code is :
```S
	/**
	 * To do a 16-bit write, the high byte must be written before the low byte.
	 *  For a 16-bit read, the low byte must be read before the high byte.
	 */
	dev->TCNTnH = val >> 8;
     3cc:	e0 ea       	ldi	r30, 0xA0	; 160
     3ce:	f0 e0       	ldi	r31, 0x00	; 0
     3d0:	80 ef       	ldi	r24, 0xF0	; 240
     3d2:	85 83       	std	Z+5, r24	; 0x05
	dev->TCNTnL = val & 0xffU;
     3d4:	8e eb       	ldi	r24, 0xBE	; 190
     3d6:	84 83       	std	Z+4, r24	; 0x04
	/* Set counter to a value to have a 250ms period after first overflow */
	ll_timer16_set_tcnt(TIMER4_DEVICE,
			    TIMER_GET_MAX_COUNTER(4) -
			    TIMER_CALC_COUNTER_VALUE(250U * USEC_PER_MSEC, 1024));
	usart_transmit('4');
     3d8:	84 e3       	ldi	r24, 0x34	; 52
     3da:	0e 94 d5 04 	call	0x9aa	; 0x9aa <usart_transmit>
```

Expected result:

- Led should be blinking at 10Hz.

```
python3 -m serial.tools.miniterm /dev/ttyACM2 500000
--- Miniterm on /dev/ttyACM2  500000,8,N,1 ---
--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
11111111:01 [1.000 s] : 1111111
00:00:00 [0.000 s] : 1111111111
00:00:01 [1.000 s] : 1111111111
00:00:02 [2.000 s] : 1111111111
00:00:03 [3.000 s] : 22222212222221222222122222212222221222222122222212222221222222122222221
00:00:04 [4.000 s] : 222222122222421222222122222212224222122222212222224122222212222221222422221
00:00:05 [5.000 s] : 222222122222421222222122222212224222122222212222224122222212222221222422221
00:00:06 [6.000 s] : 222222122222421222222122222212224222122222212222224122222212222222122422221
00:00:07 [7.000 s] : 222222122222421222222122222212224222122222212222224122222212222222122422221
00:00:08 [8.000 s] : 222222122222241222222122222212224222122222212222224122222221222222122422221
00:00:09 [9.000 s] : 222222122222241222222122222212224222122222212222224122222221222222122422221
00:00:10 [10.000 s] : 222222122222241222222122222212224222122222212222224122222221222222122422221
00:00:11 [11.000 s] : 222222122222241222222122222212224222122222212222224212222221222222122422221
00:00:12 [12.000 s] : 222222122222241222222122222212224222122222212222224212222221222222122422221
```