# execute from Ubuntu WSL
#   python3 ./scripts/pydis.py

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
        (f"avr-objdump -d {elf}", f"tmp/{name}/disassembly.asm"),
        (f"avr-objdump -S {elf}", f"tmp/{name}/disassembly_source.asm"),
        (f"avr-readelf -a {elf}", f"tmp/{name}/readelf.txt"),
        (f"avr-nm {elf}", f"tmp/{name}/nm.txt"),
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
