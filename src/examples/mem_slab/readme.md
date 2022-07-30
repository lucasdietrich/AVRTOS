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
Collected all memory slabs !
cur=M mem=0x8ed rc=0
cur=M mem=0x87d rc=0
cur=M mem=0x8cd rc=0
cur=M mem=0x8dd rc=0
cur=M mem=0x89d rc=0
cur=M mem=0x8bd rc=0
cur=M mem=0x88d rc=0
cur=M mem=0x8fd rc=0
cur=M mem=0x8ad rc=0
[T] CANARIES until @021D [found 28], MAX usage = 99 / 127 + 1 (sentinel)
[8] CANARIES until @0291 [found 16], MAX usage = 111 / 127 + 1 (sentinel)
[7] CANARIES until @032C [found 43], MAX usage = 84 / 127 + 1 (sentinel)
[6] CANARIES until @03AC [found 43], MAX usage = 84 / 127 + 1 (sentinel)
[5] CANARIES until @042C [found 43], MAX usage = 84 / 127 + 1 (sentinel)
[4] CANARIES until @04AC [found 43], MAX usage = 84 / 127 + 1 (sentinel)
[3] CANARIES until @0504 [found 3], MAX usage = 124 / 127 + 1 (sentinel)
[2] CANARIES until @05AC [found 43], MAX usage = 84 / 127 + 1 (sentinel)
[1] CANARIES until @062C [found 43], MAX usage = 84 / 127 + 1 (sentinel)
[0] CANARIES until @06AC [found 43], MAX usage = 84 / 127 + 1 (sentinel)
[I] CANARIES until @0751 [found 80], MAX usage = 40 / 120 + 1 (sentinel)
[M] CANARIES until @0935 [found 32], MAX usage = 95 / 127 + 1 (sentinel)
```