# SD Driver demo 

SD Driver demlo with STEC `SLSD256BSU` 256MB SD card, 
datasheet: <https://datasheet.datasheetarchive.com/originals/library/Datasheets-AQ6/DSAAQ00109533.pdf>

Tutorial: <http://www.rjhcoding.com/avrc-sd-interface-1.php>

Expected result
```
Card type: 1 OCR: 0x00FF8000, voltage: 0x00
CSD v1: max_read_bl_len: 512 max_write_bl_len: 512 capacity: 499712 blocks (255852544 B)
Manufacturer: 0x51
OEM/App ID: S`
Product: STEC  (rev 1.0)
Serial: 0x00000675
Manufactured: 2000/9
Block 0 updated with pattern 0x00
Block 1 already has pattern 0x00, no update needed
Block 2 already has pattern 0x00, no update needed
Block 3 already has pattern 0x00, no update needed
Block 4 already has pattern 0x00, no update needed
Block 0 read:
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
Block 1 read:
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
Block 2 read:
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
Block 3 read:
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
Block 4 read:
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
Block 0 read:
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
PRNG seed: 0xAAAAAAAA
Block 0 written with random data
Verify data:
d1 08 51 37 de 87 79 03 99 b1 e0 da 93 68 02 22 
b6 fa 44 5d 57 9c db 89 07 0c 51 f1 89 1e cb 77 
22 86 45 82 2b 62 4d 06 e6 d7 14 ea 3e 1e f0 9a 
6c 2c 73 4e da 4b f0 30 91 14 80 13 da ea 6a a0 
39 6e 5e d5 b4 95 ee 0f 05 07 9c 4c 20 5d 8a fd 
5f 99 49 d5 b5 09 a6 ac 9b 88 1c b1 3e 06 0c 65 
d2 fe e1 b2 4e d9 9c ba af 8c dd 08 1b e3 75 e6 
4f 11 a0 cf a9 c6 f8 51 3f 41 dc 6d 9b ab dd d9 
fc d6 71 82 6a 43 37 18 c4 53 12 ff a8 54 32 69 
1c 37 c1 92 9d 02 94 5d c8 cf da 83 23 b5 1d 36 
84 48 b6 c9 a6 bf 1d 71 1f dc 2c 36 19 4c fc a4 
aa 57 3d f4 b8 fe 41 68 37 04 fa ed 7c 30 8f d1 
c4 be 78 d5 44 74 01 e3 d9 5f c2 74 21 6b 9c 12 
e5 45 eb 46 e9 25 45 4a 2b 1d 0d 3f f6 4a f0 44 
57 91 ec 62 5f ab d7 e9 7e 3c 63 d1 85 59 99 f9 
e3 34 03 35 35 f4 1e 89 04 32 52 be 42 07 6b 40 
e5 ff 10 31 bb 6c 40 33 bc 8a bb ae 74 b8 ab b5 
61 95 f4 b1 61 8c 07 2b 2a 2d c6 45 7a dd 68 86 
af 30 1f 6d e9 dd 67 fe d1 57 25 1a 66 60 24 81 
25 d6 ae d6 72 4e de c7 9b 70 51 11 db 14 2a 9f 
7c ba 55 38 43 32 21 ed 86 b2 cd cf 45 a2 33 3d 
b8 ab ed 05 08 4c 8f 03 29 ee a5 06 a4 80 06 68 
8b 00 3b de 55 6c cc 00 76 a5 62 a5 e9 86 81 94 
ba de 75 d3 53 04 54 38 ee 60 64 00 59 a8 98 37 
f6 77 a9 b3 c0 5b b2 b9 91 2a 86 1b 44 98 2c 0d 
89 05 12 7a 2a da 90 5f 5a f4 2a ee 7b 16 d9 c2 
bc 7b 20 b1 4c 59 f3 c3 fc 8f 24 ee 1a 61 92 8b 
fc f1 5d 1c a8 5e fe 9c 76 40 8a f4 c7 c6 fc bd 
5e a8 1a 57 ee 02 62 20 47 4f 7e 90 0e 09 47 d2 
52 91 73 5a ad 26 a5 c8 d7 8f 2f 53 2d cb ef c9 
cc 41 8d c7 0d a2 90 a2 99 7b a8 c2 c7 14 16 ef 
8d 5c 1a da 72 f5 76 09 98 ed 54 d9 d1 79 a7 c1 
Reading all 2048 blocks...
Read 128/2048 blocks (0 MB) in 275ms (232 KB/s)
Read 256/2048 blocks (0 MB) in 552ms (231 KB/s)
Read 384/2048 blocks (0 MB) in 827ms (232 KB/s)
Read 512/2048 blocks (0 MB) in 1102ms (232 KB/s)
Read 640/2048 blocks (0 MB) in 1377ms (232 KB/s)
Read 768/2048 blocks (0 MB) in 1651ms (232 KB/s)
Read 896/2048 blocks (0 MB) in 1929ms (232 KB/s)
Read 1024/2048 blocks (0 MB) in 2207ms (231 KB/s)
Read 1152/2048 blocks (0 MB) in 2480ms (232 KB/s)
Read 1280/2048 blocks (0 MB) in 2753ms (232 KB/s)
Read 1408/2048 blocks (0 MB) in 3027ms (232 KB/s)
Read 1536/2048 blocks (0 MB) in 3301ms (232 KB/s)
Read 1664/2048 blocks (0 MB) in 3575ms (232 KB/s)
Read 1792/2048 blocks (0 MB) in 3849ms (232 KB/s)
Read 1920/2048 blocks (0 MB) in 4124ms (232 KB/s)
Read 2048/2048 blocks (1 MB) in 4398ms (232 KB/s)
All blocks read successfully
```

## Readall

With 4MHz SPI clock speed
```c
const struct spi_config spi_cfg = {
    .role        = SPI_ROLE_MASTER,
    .polarity    = SPI_CLOCK_POLARITY_RISING,
    .phase       = SPI_CLOCK_PHASE_SAMPLE,
    .prescaler   = SPI_PRESCALER_4,
    .irq_enabled = 0,
};
```

```
Card type: 1 OCR: 0x00FF8000, voltage: 0x00
Reading all 1310785537 blocks...
Read 100/1310785537 blocks (0 MB) in 216ms (231 KB/s)
Read 200/1310785537 blocks (0 MB) in 436ms (229 KB/s)
Read 300/1310785537 blocks (0 MB) in 647ms (231 KB/s)
Read 400/1310785537 blocks (0 MB) in 860ms (232 KB/s)
Read 500/1310785537 blocks (0 MB) in 1076ms (232 KB/s)
Read 600/1310785537 blocks (0 MB) in 1294ms (231 KB/s)
Read 700/1310785537 blocks (0 MB) in 1511ms (231 KB/s)
Read 800/1310785537 blocks (0 MB) in 1726ms (231 KB/s)
```