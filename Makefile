
ELF = .pio/build/Sysclock-qemu-ATmega2560/firmware.elf
CP_ELF = tmp/firmware.elf

qemu: $(ELF)
	mkdir -p tmp
	cp $(ELF) $(CP_ELF)
	qemu-system-avr -M mega2560 -bios $(CP_ELF) -s -S -nographic 