import serial
import datetime

import os
import re

from parse_ramdump import parse_ram
from parse_coredump_bin import parse_core

# todo handle several ramdump in the same run

LOCATION_DIR = "res"
COM_PORT = "COM3"
BAUDRATE = 500000

FILENAME = "ramdump.txt" # set None to have a new file every time
FILENAME_PARSED = "ramdump.parsed.txt"
FILENAME_ADDR = "addr.txt"

# https://regex101.com/r/japfJw/1
re_addr = re.compile(r"^@(?P<name>[a-zA-Z_0-9]*)\s=\s(?P<hexaddr>[0-9A-F]*)$")

if __name__ == "__main__":
    if FILENAME is None:
        now = datetime.datetime.now().strftime("%Y%m%d-%H%M%S");

        filename = f"ramdump-{now}.txt"
    else:
        filename = FILENAME

    filepath = os.path.abspath(os.path.join(LOCATION_DIR, filename))
    filepath_parsed = os.path.abspath(os.path.join(LOCATION_DIR, FILENAME_PARSED))
    filepath_addr = os.path.abspath(os.path.join(LOCATION_DIR, FILENAME_ADDR))

    ser = serial.Serial(COM_PORT, BAUDRATE)

    state = 0
    boundary = b'============\n'

    addresses = []

    with open(filepath, "bw") as fp:
        while state < 2:
            content = ser.read_until()
            
            if content == boundary:
                state += 1
                print(f"Boundary found (state = {state})")
            elif state == 1:
                fp.write(content)
                fp.flush()

            if state == 0:
                m = re_addr.match(content.decode('utf8'))
                if m:
                    addresses.append((m.group("name"), hex(int(m.group("hexaddr"), 16))))

    ser.close()

    if addresses:
        print(f"{len(addresses)} addresses found")
        with open(filepath_addr, "w+") as fp:
            for name, addr in addresses:
                fp.write(f"{name} = {addr}\n")
        
    print(f"flash dump complete : {filepath}")

    print(filepath_parsed)

    parse_ram(filepath, filepath_parsed)

    print(f"File parsed to {filepath_parsed}")