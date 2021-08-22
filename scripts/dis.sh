mkdir tmp

avr-objdump -S .pio/build/PremptMultithreadingDemo/firmware.elf > tmp/PremptMultithreadingDemo.s
avr-readelf -a .pio/build/PremptMultithreadingDemo/firmware.elf > tmp/PremptMultithreadingDemo.readelf.txt

avr-objdump -S .pio/build/CoopMultithreadingDemo/firmware.elf > tmp/CoopMultithreadingDemo.s
avr-readelf -a .pio/build/CoopMultithreadingDemo/firmware.elf > tmp/CoopMultithreadingDemo.readelf.txt

avr-objdump -S .pio/build/MutexMultithreadingDemo/firmware.elf > tmp/MutexMultithreadingDemo.s
avr-readelf -a .pio/build/MutexMultithreadingDemo/firmware.elf > tmp/MutexMultithreadingDemo.readelf.txt
avr-nm .pio/build/MutexMultithreadingDemo/firmware.elf > tmp/MutexMultithreadingDemo.nm.txt

avr-objdump -S .pio/build/SemaphoreMultithreadingDemo/firmware.elf > tmp/SemaphoreMultithreadingDemo.s
avr-readelf -a .pio/build/SemaphoreMultithreadingDemo/firmware.elf > tmp/SemaphoreMultithreadingDemo.readelf.txt
avr-nm .pio/build/SemaphoreMultithreadingDemo/firmware.elf > tmp/SemaphoreMultithreadingDemo.nm.txt

avr-objdump -S .pio/build/KernelTest/firmware.elf > tmp/KernelTest.s
avr-readelf -a .pio/build/KernelTest/firmware.elf > tmp/KernelTest.readelf.txt
avr-nm .pio/build/KernelTest/firmware.elf > tmp/KernelTest.nm.txt

avr-objdump -S .pio/build/WorkQueue/firmware.elf > tmp/WorkQueue.s
avr-readelf -a .pio/build/WorkQueue/firmware.elf > tmp/WorkQueue.readelf.txt
avr-nm .pio/build/WorkQueue/firmware.elf > tmp/WorkQueue.nm.txt

avr-objdump -S .pio/build/SchedLock/firmware.elf > tmp/SchedLock.s
avr-readelf -a .pio/build/SchedLock/firmware.elf > tmp/SchedLock.readelf.txt
avr-nm .pio/build/SchedLock/firmware.elf > tmp/SchedLock.nm.txt

avr-objdump -S .pio/build/Canaries/firmware.elf > tmp/Canaries.s
avr-readelf -a .pio/build/Canaries/firmware.elf > tmp/Canaries.readelf.txt