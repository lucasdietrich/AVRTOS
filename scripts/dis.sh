avr-objdump -S .pio/build/PremptMultithreadingDemo/firmware.elf > res/tmp/PremptMultithreadingDemo.s
avr-readelf -a .pio/build/PremptMultithreadingDemo/firmware.elf > res/tmp/PremptMultithreadingDemo.readelf.txt

avr-objdump -S .pio/build/CoopMultithreadingDemo/firmware.elf > res/tmp/CoopMultithreadingDemo.s
avr-readelf -a .pio/build/CoopMultithreadingDemo/firmware.elf > res/tmp/CoopMultithreadingDemo.readelf.txt

avr-objdump -S .pio/build/MutexMultithreadingDemo/firmware.elf > res/tmp/MutexMultithreadingDemo.s
avr-readelf -a .pio/build/MutexMultithreadingDemo/firmware.elf > res/tmp/MutexMultithreadingDemo.readelf.txt

avr-objdump -S .pio/build/KernelTest/firmware.elf > res/tmp/KernelTest.s
avr-readelf -a .pio/build/KernelTest/firmware.elf > res/tmp/KernelTest.readelf.txt
avr-nm .pio/build/KernelTest/firmware.elf > res/tmp/KernelTest.nm.txt