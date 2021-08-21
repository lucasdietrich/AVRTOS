avr-objdump -S .pio/build/PremptMultithreadingDemo/firmware.elf > res/tmp/PremptMultithreadingDemo.s
avr-readelf -a .pio/build/PremptMultithreadingDemo/firmware.elf > res/tmp/PremptMultithreadingDemo.readelf.txt

avr-objdump -S .pio/build/CoopMultithreadingDemo/firmware.elf > res/tmp/CoopMultithreadingDemo.s
avr-readelf -a .pio/build/CoopMultithreadingDemo/firmware.elf > res/tmp/CoopMultithreadingDemo.readelf.txt

avr-objdump -S .pio/build/MutexMultithreadingDemo/firmware.elf > res/tmp/MutexMultithreadingDemo.s
avr-readelf -a .pio/build/MutexMultithreadingDemo/firmware.elf > res/tmp/MutexMultithreadingDemo.readelf.txt
avr-nm .pio/build/MutexMultithreadingDemo/firmware.elf > res/tmp/MutexMultithreadingDemo.nm.txt

avr-objdump -S .pio/build/SemaphoreMultithreadingDemo/firmware.elf > res/tmp/SemaphoreMultithreadingDemo.s
avr-readelf -a .pio/build/SemaphoreMultithreadingDemo/firmware.elf > res/tmp/SemaphoreMultithreadingDemo.readelf.txt
avr-nm .pio/build/SemaphoreMultithreadingDemo/firmware.elf > res/tmp/SemaphoreMultithreadingDemo.nm.txt

avr-objdump -S .pio/build/KernelTest/firmware.elf > res/tmp/KernelTest.s
avr-readelf -a .pio/build/KernelTest/firmware.elf > res/tmp/KernelTest.readelf.txt
avr-nm .pio/build/KernelTest/firmware.elf > res/tmp/KernelTest.nm.txt

avr-objdump -S .pio/build/WorkQueue/firmware.elf > res/tmp/WorkQueue.s
avr-readelf -a .pio/build/WorkQueue/firmware.elf > res/tmp/WorkQueue.readelf.txt
avr-nm .pio/build/WorkQueue/firmware.elf > res/tmp/WorkQueue.nm.txt

avr-objdump -S .pio/build/SchedLock/firmware.elf > res/tmp/SchedLock.s
avr-readelf -a .pio/build/SchedLock/firmware.elf > res/tmp/SchedLock.readelf.txt
avr-nm .pio/build/SchedLock/firmware.elf > res/tmp/SchedLock.nm.txt

avr-objdump -S .pio/build/Canaries/firmware.elf > res/tmp/Canaries.s
avr-readelf -a .pio/build/Canaries/firmware.elf > res/tmp/Canaries.readelf.txt