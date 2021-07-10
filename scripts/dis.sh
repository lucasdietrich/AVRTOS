avr-objdump -S .pio/build/InterruptYield/firmware.elf > res/tmp/InterruptYield.s
avr-readelf -a .pio/build/InterruptYield/firmware.elf > res/tmp/InterruptYield.readelf.txt