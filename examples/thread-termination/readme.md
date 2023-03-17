Expected result `atmega2560` with QEMU

```
Memory region         Used Size  Region Size  %age Used
            text:        5922 B       256 KB      2.26%
            data:        1232 B         8 KB     15.04%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
```

```
===== k_thread =====
C 0x0484 READY   P 1 ____ : SP 15/256:0x0305
T 0x0496 READY   P 1 ____ : SP 22/256:0x0405
I 0x04A8 IDLE    P 1 ____ : SP 22/120:0x047D
M 0x04BA PENDING P 1 ____ : SP 36/512:0x06CF
[C] CANARIES until @02E1 [found 219], MAX usage = 37 / 256
[T] CANARIES until @03EF [found 233], MAX usage = 23 / 256
[I] CANARIES until @0467 [found 97], MAX usage = 23 / 120
[M] CANARIES until @06AC [found 476], MAX usage = 36 / 512
Hello !
Hello !
Hello !
Hello !
Hello !
Thread terminated : 0054
Thread started again
Hello !
Hello !
Hello !
Hello !
```