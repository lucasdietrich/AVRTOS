Expected result `atmega2560` with QEMU

```
Memory region         Used Size  Region Size  %age Used
            text:        8110 B       256 KB      3.09%
            data:        1263 B         8 KB     15.42%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
```

```
===== k_thread =====
1 0x049A READY   P 1 ____ : SP 22/256:0x02FF
1 0x04AC READY   P 1 ____ : SP 22/256:0x03FF
I 0x04BE IDLE    P 1 ____ : SP 22/120:0x0477
M 0x04D0 READY   P 1 ____ : SP 0/512:0x06EE
00:00:00 [0.002 s] : Startup
00:00:00 [0.004 s] :  6 64ms - 4 32ms - 0 s - 100000 s (timestamp)
00:00:01 [1.006 s] :  1006 64ms - 1006 32ms - 1 s - 100001 s (timestamp)
00:00:02 [2.006 s] :  2006 64ms - 2006 32ms - 2 s - 100002 s (timestamp)
00:00:03 [3.006 s] :  3006 64ms - 3006 32ms - 3 s - 100003 s (timestamp)
00:00:04 [4.006 s] :  4006 64ms - 4006 32ms - 4 s - 100004 s (timestamp)
00:00:05 [5.006 s] :  5006 64ms - 5006 32ms - 5 s - 200000 s (timestamp)
00:00:06 [6.006 s] :  6006 64ms - 6006 32ms - 6 s - 200001 s (timestamp)
00:00:07 [7.006 s] :  7006 64ms - 7006 32ms - 7 s - 200002 s (timestamp)
```