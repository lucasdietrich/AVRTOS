Expected result `atmega2560` with QEMU

- Send data through serial

```
Memory region         Used Size  Region Size  %age Used
            text:        5400 B       256 KB      2.06%
            data:        1014 B         8 KB     12.38%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
```

```
===== k_thread =====
W 0x03B8 READY   P 1 ____ : SP 22/256:0x0305
I 0x03CA IDLE    P 1 ____ : SP 22/120:0x037D
M 0x03DC READY   P 1 ____ : SP 0/512:0x05F5
k_poll_signal returned err = -2
sk_poll_signal returned err = 0
signal value = 115
sk_poll_signal returned err = 0
signal value = 115
qk_poll_signal returned err = 0
signal value = 113
dk_poll_signal returned err = 0
signal value = 100
dk_poll_signal returned err = 0
signal value = 100
dk_poll_signal returned err = 0
signal value = 100
dk_poll_signal returned err = 0
signal value = 100
sk_poll_signal returned err = 0
signal value = 115
sk_poll_signal returned err = 0
signal value = 115
qk_poll_signal returned err = 0
signal value = 113
dk_poll_signal returned err = 0
signal value = 100
```