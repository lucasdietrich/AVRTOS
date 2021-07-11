import re
import struct

from typing import List


class Core:
    def __init__(self):
        self.registers = []

    def SP(self) -> int:
        return self.registers[0]

    def PC(self) -> int:
        return self.registers[1]

    def SREG(self) -> int:
        return self.registers[-1]

    def r(self, i: int) -> int:
        return self.registers[i + 2]

    def __repr__(self):
        return f"Core\n" \
               f"  SP={self.SP():0{4}X} PC={self.PC():0{4}X} SREG={self.SREG():0{2}X} " + \
               (" ".join(f"r{i}={self.r(i)}" for i in range(32)))


re_coredump = re.compile(b"<<<<.{37}>>>>")


def parse_core(filename: str) -> List[Core]:
    with open(filename, "br") as fp:
        content = fp.read()

    match = re_coredump.findall(content)

    cores = []

    print(match)

    for parsed in match:
        data = parsed[4:41]

        # https://docs.python.org/3/library/struct.html
        core = Core()
        core.registers = struct.unpack("HHB" + "B" * 32, data)
        cores.append(core)

        print(core.registers, core)

    return cores


def decode(core: bytes):
    return


if __name__ == "__main__":
    import sys
    if len(sys.argv) == 2:

        parse_core(sys.argv[1])
    else:
        raise Exception("argument problem")