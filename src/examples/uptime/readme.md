Expected result `atmega2560` with QEMU

```
Memory region         Used Size  Region Size  %age Used
            text:        8594 B       256 KB      3.28%
            data:        1275 B         8 KB     15.56%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
```

```
===== k_thread =====
A 0x04B0 READY   P 1 ____ : SP 22/512:0x03FF
I 0x04C2 IDLE    P 1 ____ : SP 22/120:0x0477
M 0x04D4 READY   P 1 ____ : SP 0/512:0x06FA
00:00:01 [1.000 s] : now = 0 1000
00:00:02 [2.000 s] : now = 0 2000
00:00:03 [3.000 s] : now = 0 3000
00:00:04 [4.000 s] : now = 0 4000
00:00:05 [5.000 s] : now = 0 5000
[A] CANARIES until @038B [found 395], MAX usage = 117 / 512
[I] CANARIES until @0461 [found 97], MAX usage = 23 / 120
[M] CANARIES until @06CD [found 466], MAX usage = 46 / 512
00:00:06 [6.000 s] : now = 0 6000
00:00:07 [7.000 s] : now = 0 7000
```