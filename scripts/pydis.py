# require avr-gcc toolchain

import os
import os.path
import subprocess

def create_dir(path: str):
    # create directory
    if os.path.exists(path):
        if os.path.isfile(path):
            raise Exception(f"{path} is a file !")
    else:
        os.mkdir(path)

def dis(name: str, elf: str):
    commands = [
        (f"avr-objdump -S {elf}", f"tmp/{name}/objdump_src.s"),
        (f"avr-objdump -d {elf}", f"tmp/{name}/objdump.s"),
        (f"avr-objdump -D {elf}", f"tmp/{name}/objdump_all.s"),
        (f"avr-objdump -h {elf}", f"tmp/{name}/objdump_sections.s"),
        (f"avr-readelf -a {elf}", f"tmp/{name}/readelf.txt"),
        (f"avr-readelf -x .data {elf}", f"tmp/{name}/section_data.txt"),
        (f"avr-readelf -x .bss {elf}", f"tmp/{name}/section_bss.txt"),
        (f"avr-readelf -x .noinit {elf}", f"tmp/{name}/section_noinit.txt"),
        (f"avr-readelf -x .text {elf}", f"tmp/{name}/section_text.txt"),
        (f"avr-nm --print-size --size-sort --radix=x {elf}", f"tmp/{name}/nm-size.txt"),
        (f"avr-nm {elf}", f"tmp/{name}/nm.txt"),
        # (f"grep -E '([a-f0-9]*)\s([a-f0-9]*)\s([^tT])' tmp/{name}/nm-size.txt", f"tmp/{name}/nm-size-sram.txt"),
        # (f"grep -E '([a-f0-9]*)\s([a-f0-9]*)\s([^tT])' tmp/{name}/nm-size.txt", f"tmp/{name}/nm-size-rom.txt")
    ]

    for command in commands:
        process = subprocess.Popen(command[0].split(), stdout=open(command[1], "w+"), cwd=".")
        output, error = process.communicate()

        if output or error:
            print(output, error)

    return commands

if __name__ == "__main__":
    DIR = "./.pio/build/"
    TMP = "./tmp/"

    create_dir(TMP)

    # list projects
    for name in os.listdir(".pio/build/"):
        elf = os.path.join(DIR, f"{name}/firmware.elf")
        if os.path.exists(elf) and os.path.isfile(elf):
            create_dir(os.path.join(TMP, name))
            commands = dis(name, elf)
            print(f"- {name} :\n\t" + '\n\t'.join(map(lambda cmd: cmd[1], commands)))
