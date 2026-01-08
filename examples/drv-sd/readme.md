# SD Driver demo 

SD Driver demlo with STEC `SLSD256BSU` 256MB SD card, 
datasheet: <https://datasheet.datasheetarchive.com/originals/library/Datasheets-AQ6/DSAAQ00109533.pdf>

Tutorial: <http://www.rjhcoding.com/avrc-sd-interface-1.php>

Expected result
```
SD card driver demo
CMD0 arg=0x00000000
R1: 0x01
CMD8 arg=0x000001AA
R1: 0x05
CMD58 arg=0x00000000
R1: 0x01
R3/R7: 00 FF 80 00
CMD55 arg=0x00000000
R1: 0x01
CMD41 arg=0x00000000
R1: 0x01
CMD55 arg=0x00000000
R1: 0x01
CMD41 arg=0x00000000
R1: 0x00
CMD16 arg=0x00000200
R1: 0x00
SD card initialized
Card type: 1, version: 1
OCR: 0x00FF8000, voltage: 0x00
R1: 0x00
Block 0 read:
b2 28 bb 45 38 c8 dc c3 1e a8 73 a6 45 0d 2b 28 
92 91 8d 87 4b 43 1c 7e 04 0b 63 0a 19 b3 af 9c 
fb fc 5d 35 ed 4f 60 5e 43 e8 53 8c 5d 3d da 28 
ba 13 da d2 5e 45 51 b3 d7 9d 8d a2 37 2f bf 38 
a2 21 4a bc f1 0f d1 c0 38 4e 7d 77 76 fa 03 47 
53 de 8a 44 37 d7 5a c8 a4 d1 a1 9d bf e4 02 74 
cd 69 d4 ad 2d 77 f6 eb a9 8b 40 5a 12 5e 99 20 
f4 20 3c 39 0e fb 63 64 ab b6 39 50 4c cd 9a 3a 
df d4 0b 78 a8 ae 1b 70 cf 09 62 93 b4 0f f5 7f 
5f 08 e3 27 89 bc 11 b2 87 9d bb ed 15 47 cc 59 
de 3a 7c 46 08 8f 6a ae 30 51 95 ef d9 d3 0c 83 
56 1e 42 e6 d5 00 74 fe 14 08 00 14 5e 5d 9b 99 
c1 ec 57 e8 fd 81 c1 b7 e9 0a 8e 21 72 3e 3a 7d 
50 0d 68 7d fb d1 1d a4 bd 66 b4 32 36 60 40 8e 
fb a3 47 2e 87 42 a1 13 bf 91 bd d3 6e 4a 91 c7 
61 7b bf f5 1a c4 e7 7f 14 26 56 fb 03 56 95 3d 
c6 b3 ea 98 99 95 54 de f9 b1 c4 89 59 4b ce 60 
96 49 3b fc e0 25 c9 68 85 ae df 9f 52 c2 a2 c1 
01 b2 e8 de e6 8b 16 11 a3 45 e5 cd 96 d5 58 ec 
7a 4d 8b 8a d5 70 85 32 3f b7 b8 a0 0f 11 5f 1e 
8f 73 db b0 3c c3 8c eb 83 1a 62 cc 5c 18 1b 3a 
72 2a c3 b6 9f 33 98 a6 9e bd ba 28 d1 31 f5 ed 
05 5b 84 1b c8 27 85 fb 0a 72 bd 90 6c 31 ce 50 
5b d5 23 78 da fd 98 ef 1d 95 ea 47 e6 ea c5 9d 
cf b4 c9 6a 98 6d 24 74 a8 8a 26 5a 2c c8 36 19 
d3 13 d8 ac c4 3c 77 f5 11 24 ca 20 c3 a5 37 6e 
db c3 cb 65 84 96 01 70 5c 30 cc 41 e0 8a 30 22 
c8 48 10 b2 95 9c df c5 90 7f 2f 68 40 c8 af 84 
f8 5c 6f b9 dd 10 1c 5e 84 43 bc 69 22 64 0c 49 
72 cd 7b cd 6f 83 89 72 5c 21 7f 82 79 ff fa c4 
27 6b c5 77 1f 8e c8 7a 8f 0d 71 4c df 9e 90 8d 
70 57 4b f1 08 56 f4 32 24 70 3b fe f8 0a c3 b4 
PRNG seed: 0xB228BB45
R1: 0x00
Block 0 written with random data
R1: 0x00
Verify data:
58 36 5d d1 5d 3e eb 57 e1 77 7e ab a8 e7 3e bb 
f9 1c 25 9e 21 85 da 1c e9 40 36 f1 62 64 83 d3 
30 af 5d 1b 6d 04 80 cc fb 8c 7d 95 4a a8 6d b7 
a5 4e 6f ff 9d a7 13 f1 1b 9e 7e a2 f9 d5 50 55 
2b 40 86 4d 82 33 3f 35 1f e0 06 e6 da e4 a8 40 
4b 65 4c 2a da 9f cf 73 ef d6 81 66 77 5a b0 db 
7d 51 f8 a3 a6 74 e6 51 2d 65 e1 a1 54 c9 14 56 
f9 94 91 bd 8b f3 ec cf ba e6 93 ed 7a 31 cb 16 
65 f6 f9 63 08 bb 73 35 95 98 68 64 2a a1 0e c7 
93 5c d0 72 37 d3 a2 ba 59 65 d0 40 bd 56 a6 9d 
98 d8 b1 ed 55 0c 13 a7 c0 fa 69 45 66 b9 51 23 
ef df 82 ff a5 46 e8 af 73 dc eb 9b 56 c4 d9 3f 
0d 96 e4 f6 a3 bc ca 6b 19 47 f2 b2 ad 9a ff ae 
0f 50 cd 10 3c a3 5e 02 df 35 30 b5 d7 01 96 bf 
d2 3c 23 e0 1e 71 e9 36 ed 58 b5 b2 2c 92 dd a2 
32 e2 19 e9 6f 10 54 a3 ca ab 7d 8b fb 70 2b f4 
f5 12 54 0a 6a e2 5a b9 66 fb 91 21 e2 48 d2 5a 
11 c7 68 25 46 29 bc b8 36 73 41 52 69 22 5e 03 
be 42 35 d8 ff 63 be 02 49 81 83 af 53 9e 07 97 
5f e9 2c ca 0e 9a c1 32 2e b7 12 49 39 b8 4e 27 
65 54 7d aa c9 56 5f e7 45 63 4f 41 b9 52 0c 41 
45 8e 3c e5 92 1d b5 84 bb 1c 5d a9 5a 2b a5 91 
11 c4 41 bc 97 a7 60 21 1d 2d 8a fd d3 50 53 b2 
52 c4 cf d5 a1 16 b4 ef 16 95 1a bd 5a 94 6a 42 
d6 39 21 e0 22 cf d6 cf 08 77 4e a5 f6 b7 9c 95 
a5 e3 99 83 ba de b4 5e 13 b4 96 17 14 a8 b0 97 
48 07 1d ee 2d 27 c5 35 2d a1 3e 0c c0 ca 38 32 
ca 4c 4c 60 02 68 08 42 97 a4 1c 64 be f1 8c d9 
42 f1 5d 81 8d 0d 48 99 51 32 67 dc a6 33 ed 5c 
a4 a1 12 05 db d8 64 64 49 8d d4 c0 21 15 6c 7e 
de 96 15 a1 ab 7b a5 07 7a 7b 62 38 29 79 31 27 
76 d4 d6 42 41 d3 08 ba 1b 52 46 4c c9 4f 21 0c 
Demo completed successfully
```