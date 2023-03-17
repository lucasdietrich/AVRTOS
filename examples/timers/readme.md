Expected result `atmega2560` with QEMU

```
Memory region         Used Size  Region Size  %age Used
            text:        7918 B       256 KB      3.02%
            data:        2062 B         8 KB     25.17%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
```

```
===== k_thread =====
A 0x079C READY   P 1 ____ : SP 22/256:0x02FF
C 0x07AE READY   C 0 ____ : SP 22/512:0x04FF
I 0x07C0 IDLE    P 1 ____ : SP 22/137:0x058C
M 0x07D2 READY   P 1 ____ : SP 0/512:0x0A0B
W 0x07E4 READY   P 1 ____ : SP 22/512:0x0792
MAIN : 0
[A] CANARIES until @02E4 [found 227], MAX usage = 28 / 255 + 1 (sentinel)
[C] CANARIES until @04DA [found 473], MAX usage = 38 / 511 + 1 (sentinel)
[I] CANARIES until @0576 [found 113], MAX usage = 23 / 136 + 1 (sentinel)
[M] CANARIES until @09E7 [found 474], MAX usage = 37 / 511 + 1 (sentinel)
[W] CANARIES until @077C [found 488], MAX usage = 23 / 511 + 1 (sentinel)
WORKQUEUE : 3
WORKQUEUE : 5
WORKQUEUE : 7
WORKQUEUE : 9
MAIN : 10
WORKQUEUE : 11
WORKQUEUE : 13
WORKQUEUE : 15
WORKQUEUE : 17
WORKQUEUE : 19
MAIN : 20
WORKQUEUE : 21
WORKQUEUE : 23
WORKQUEUE : 25
WORKQUEUE : 27
WORKQUEUE : 29
MAIN : 30
WORKQUEUE : 31
WORKQUEUE : 33
WORKQUEUE : 35
WORKQUEUE : 37
WORKQUEUE : 39
MAIN : 40
WORKQUEUE : 41
WORKQUEUE : 43
WORKQUEUE : 45
WORKQUEUE : 47
WORKQUEUE : 49
MAIN : 50
WORKQUEUE : 51
WORKQUEUE : 53
WORKQUEUE : 55
WORKQUEUE : 57
WORKQUEUE : 59
MAIN : 50
WORKQUEUE : 61
WORKQUEUE : 63
WORKQUEUE : 65
```