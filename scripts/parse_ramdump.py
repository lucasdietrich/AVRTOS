from __future__ import annotations

import math
import string
import re
import logging
from typing import Union

FILE = "res/ramdump.txt"
FILE_PARSED = "res/ramdump.parsed.txt"

# init config
logger = logging.getLogger("ramdump.parser")
logging.basicConfig()
logger.setLevel(logging.INFO)

# https://regex101.com/r/gI1ZSW/1
line_re = re.compile(r"^((?P<addr>[\dA-F]{4})\s:\s(?P<val>[\dA-F]{2}))(?P<SP>\s\s\<--\shere)?$")


class Hex:
    def __init__(self, val: Union[int, str], rjust_n: int = 2):
        if isinstance(val, str):
            val = int(val, 16)
        elif isinstance(val, int):
            pass
        else:
            raise ValueError("invalid argument val")

        self.val = val
        self.rjust_n = rjust_n

    def as_printable(self) -> str:
        char = chr(self.val)

        is_printable = char in string.printable[:-5]  # skipping \t\n\r\x0b\x0c

        if is_printable:
            return char
        else:
            return "."

    def __eq__(self, other: Hex):
        return self.val == other.val

    def __str__(self):
        return hex(self.val)[2:].upper().rjust(self.rjust_n, '0')

    def __repr__(self):
        return str(self)


class Addr(Hex):
    def __init__(self, val: Union[int, str]):
        super(Addr, self).__init__(val, 4)

    def __add__(self, other: Union[Addr, int]):
        if isinstance(other, Hex):
            return Addr(self.val + other.val)
        elif isinstance(other, int):
            return Addr(self.val + other)
        else:
            raise ValueError(f"invalid summation with {other}")


class RAM:
    def __init__(self, SP: Addr = None):
        self.SP = SP
        self.ram = []

    # warning "__setitem__ key" (addr) and "__getitem__ key" (index) does not represent the same object !!
    def __setitem__(self, addr: Addr, val: Hex):
        self.ram.append((addr, val))

    def __getitem__(self, index: int):
        return self.ram[index]

    def __len__(self):
        return len(self.ram)

    def display(self, group: int = 0) -> str:
        content = ""
        for addr, val in self.ram:
            issp = " [SP]" if self.SP == addr else ""
            val_text = val.as_printable()

            content += f"{addr} : {val} {val_text}{issp}\n"

        return content

    def display_group(self, group_size: int = 8) -> str:
        size = len(self)

        content = ""

        for i in range(size // group_size):
            i_base = i*group_size
            base_addr = self[i_base][0]
            to_addr = base_addr + group_size + (-1)

            str_vals = [
                str(self[j][1]) for j in range(i_base, i_base + group_size)
            ]

            char_vals = [
                self[j][1].as_printable() for j in range(i_base, i_base + group_size)
            ]

            content += f"{base_addr} > {to_addr} : " + " ".join(str_vals) + "  " + "".join(char_vals) + "\n"

        return content



def parse(filename: str, filename_parsed: str):
    ram = RAM()

    # read file
    with open(filename, "r") as fp:
        while True:
            line = fp.readline()

            line.rstrip('\n')

            if line == "":
                break

            m = line_re.match(line)

            if m:
                groups = m.groupdict()

                addr, val, sp = Addr(groups["addr"]), Hex(groups["val"]), groups["SP"]

                ram[addr] = val

                if sp is not None:
                    logger.info(f"SP flag found at addr {groups['addr']}")
                    ram.SP = addr
            else:
                logger.error("invalid ramdump file")
                break

    display = ram.display_group(16)

    with open(filename_parsed, "w+") as fp:
        fp.write(display)

    return display

if __name__ == "__main__":

    print(parse(FILE, FILE_PARSED))
