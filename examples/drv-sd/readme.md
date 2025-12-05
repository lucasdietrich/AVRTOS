# SD Driver demo 

SD Driver demlo with STEC `SLSD256BSU` 256MB SD card, 
datasheet: <https://datasheet.datasheetarchive.com/originals/library/Datasheets-AQ6/DSAAQ00109533.pdf>

Tutorial: <http://www.rjhcoding.com/avrc-sd-interface-1.php>

Expected result
```
SD Driver demo started
40 00 00 00 00 95 
51 00 00 00 00 55 
40 00 00 00 00 95 
CMD0 response: 0x01
        SD card is in IDLE state
CMD8 prepared:
48 00 00 01 aa 87 
        SD card CMD8 illegal command - Ver1.X card
CMD58 prepared:
7a 00 00 00 00 fd 
CMD58 response OCR register: 0x01
00 ff 80 00 
CMD55 prepared:
77 00 00 00 00 65 
CMD55 response: 0x01
ACMD41 prepared:
69 00 00 00 00 e5 
CMD41 response OCR register: 0x01
ff ff ff ff 
CMD55 prepared:
77 00 00 00 00 65 
CMD55 response: 0x01
ACMD41 prepared:
69 00 00 00 00 e5 
CMD41 response OCR register: 0x00
ff ff ff ff 
SD card initialized successfully
CMD16 prepared:
50 00 00 02 00 15 
CMD16 response: 0x00
CMD17 prepared:
51 00 00 00 00 55 
CMD17 response: 0x00
Data token received: 0xFE
Receiving block data...
Received CRC: 0x4721
Block data received:
ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee ee 
CMD24 prepared:
58 00 00 00 00 6f 
Data block sent, waiting for data response...
Write completed successfully response: 0xE5
Data accepted by SD card
```