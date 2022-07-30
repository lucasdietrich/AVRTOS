Expected output: `atmega2560` with QEMU

```
Memory region         Used Size  Region Size  %age Used
            text:        5567 B       256 KB      2.12%
            data:        1785 B         8 KB     21.79%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
[100%] Built target sample_interrupt_yield
===== k_thread =====
R 0x0686 READY   P 1 ____ : SP 22/512:0x0407
W 0x0698 READY   P 1 ____ : SP 22/512:0x0607
I 0x06AA IDLE    P 1 ____ : SP 22/120:0x067F
M 0x06BC READY   P 1 ____ : SP 0/512:0x08F8
Got a letter from the UART : o
Got a letter from the UART : z
XGot a letter from the UART : d
Got a letter from the UART : h
Got a letter from the UART : f
Got a letter from the UART : o
Got a letter from the UART : q
Got a letter from the UART : s
Got a letter from the UART : i
Got a letter from the UART : d
Got a letter from the UART : h
Got a letter from the UART : q
XXXXXXXXXXXXGot a letter from the UART : p
Got a letter from the UART : f
Got a letter from the UART : h
XGot a letter from the UART : d
XXXXXXGot a letter from the UART : s
XXXXXXXGot a letter from the UART : i
Got a letter from the UART : o
Got a letter from the UART : h
Got a letter from the UART : o
Got a letter from the UART : q
Got a letter from the UART : s
Got a letter from the UART : d
Got a letter from the UART : q
```