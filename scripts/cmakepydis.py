from pydis import dis

if __name__ == "__main__":
    dis("cmake", "build/blinking-led.elf")