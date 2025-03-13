Expected result with `atmega2560` with QEMU

Compilation
```
Memory region         Used Size  Region Size  %age Used
            text:       10715 B       256 KB      4.09%
            data:        1940 B         8 KB     23.68%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
```

After few seconds :
```
===== k_thread =====
0 0x0200 READY   P 1 ____ : SP 22/128:0x0374
1 0x0212 READY   P 1 ____ : SP 22/128:0x03F4
2 0x0224 READY   P 1 ____ : SP 22/128:0x0474
3 0x0236 READY   P 1 ____ : SP 22/128:0x04F4
4 0x0248 READY   P 1 ____ : SP 22/128:0x0574
5 0x025A READY   P 1 ____ : SP 22/128:0x05F4
6 0x026C READY   P 1 ____ : SP 22/128:0x0674
7 0x027E READY   P 1 ____ : SP 22/128:0x06F4
8 0x0290 READY   P 1 ____ : SP 22/128:0x0774
9 0x02A2 READY   P 1 ____ : SP 22/128:0x07F4
T 0x02B4 READY   C 1 ____ : SP 22/128:0x0874
M 0x02C6 READY   C 1 ____ : SP 0/128:0x0A27
I 0x02D8 IDLE    P 1 ____ : SP 22/121:0x08ED
00:00:00 [0.000 s] : 
00:00:01 [1.000 s] : 
00:00:02 [2.000 s] : 
00:00:03 [3.000 s] : 
Collected 1 more memory slab (1/9) !
00:00:04 [4.000 s] : 
00:00:05 [5.000 s] : 
00:00:06 [6.000 s] : 
Collected 1 more memory slab (2/9) !
00:00:07 [7.000 s] : 
00:00:08 [8.000 s] : 
00:00:09 [9.000 s] : 
Collected 1 more memory slab (3/9) !
00:00:10 [10.000 s] : 
00:00:11 [11.000 s] : 
00:00:12 [12.000 s] : 
Collected 1 more memory slab (4/9) !
00:00:13 [13.000 s] : 
00:00:14 [14.000 s] : 
00:00:15 [15.000 s] : 
Collected 1 more memory slab (5/9) !
00:00:16 [16.000 s] : 
00:00:17 [17.000 s] : 
00:00:18 [18.000 s] : 
Collected 1 more memory slab (6/9) !
00:00:19 [19.000 s] : 
00:00:20 [20.000 s] : 
00:00:21 [21.000 s] : 
Collected 1 more memory slab (7/9) !
00:00:22 [22.000 s] : 
00:00:23 [23.000 s] : 
00:00:24 [24.000 s] : 
Collected 1 more memory slab (8/9) !
00:00:25 [25.000 s] : 
00:00:26 [26.000 s] : 
00:00:27 [27.000 s] : 
00:00:28 [28.000 s] : 
Collected 1 more memory slab (9/9) !
Collected all memory slabs !

[0] CANARIES until @0334 [found 62], MAX usage = 65 / 127 + 1 (sent)
[1] CANARIES until @03B4 [found 62], MAX usage = 65 / 127 + 1 (sent)
[2] CANARIES until @0434 [found 62], MAX usage = 65 / 127 + 1 (sent)
[3] CANARIES until @04B4 [found 62], MAX usage = 65 / 127 + 1 (sent)
[4] CANARIES until @0534 [found 62], MAX usage = 65 / 127 + 1 (sent)
[5] CANARIES until @05B4 [found 62], MAX usage = 65 / 127 + 1 (sent)
[6] CANARIES until @0634 [found 62], MAX usage = 65 / 127 + 1 (sent)
[7] CANARIES until @06B4 [found 62], MAX usage = 65 / 127 + 1 (sent)
[8] CANARIES until @0734 [found 62], MAX usage = 65 / 127 + 1 (sent)
[9] CANARIES until @07B4 [found 62], MAX usage = 65 / 127 + 1 (sent)
[T] CANARIES until @080D [found 23], MAX usage = 104 / 127 + 1 (sent)
[M] CANARIES until @09D4 [found 43], MAX usage = 84 / 127 + 1 (sent)
[I] CANARIES until @08C6 [found 80], MAX usage = 40 / 120 + 1 (sent)
```