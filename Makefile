build:
	cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/avr6-atmega2560.cmake
	make -C build
# -–no-print-directory

clean:
	rm -rf build

qemu:
	make -C build qemu