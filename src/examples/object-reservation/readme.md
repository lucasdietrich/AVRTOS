Expected result `atmega2560` with QEMU

```
Memory region         Used Size  Region Size  %age Used
            text:        5648 B       256 KB      2.15%
            data:        1510 B         8 KB     18.43%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
```

```
===== k_thread =====
C 0x058A READY   P 1 ____ : SP 22/256:0x0306
B 0x059C READY   P 1 ____ : SP 22/256:0x0406
A 0x05AE READY   P 1 ____ : SP 22/256:0x0506
I 0x05C0 IDLE    P 1 ____ : SP 22/120:0x0584
M 0x05D2 READY   P 1 ____ : SP 0/512:0x07E5
}M~>CC: starting
~>BB: starting
~>AA: starting
~>I.!M>MM{@C~>C}CC: locked the mutex !
~>I.!B>BB: didn't get the mutex !
~>I.!A>AA: didn't get the mutex !
~>I.>I.>I.>I.>I.>I
```