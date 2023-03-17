Expected result `atmega2560` with QEMU

```
Memory region         Used Size  Region Size  %age Used
            text:        4720 B       256 KB      1.80%
            data:        1780 B         8 KB     21.73%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
```

```
===== k_thread =====
4 0x0686 READY   P 1 ____ : SP 22/256:0x0306
3 0x0698 READY   P 1 ____ : SP 22/256:0x0406
2 0x06AA READY   P 1 ____ : SP 22/256:0x0506
1 0x06BC READY   P 1 ____ : SP 22/256:0x0606
I 0x06CE IDLE    P 1 ____ : SP 22/120:0x067E
M 0x06E0 READY   P 1 ____ : SP 0/512:0x08F3
4 : Got the mutex !
3 : Got the mutex !
2 : Didn't get the mutex ...
1 : Didn't get the mutex ...
```