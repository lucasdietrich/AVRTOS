import re

from typing import List


class Core:
    def __init__(self, registers: List[int]):
        self.registers = registers

    def SP(self) -> str:
        return hex(self.registers[0] << + self.registers[1])

    def PC(self) -> str:
        return hex(self.registers[2] << + self.registers[3])

    def SREG(self) -> str:
        return hex(self.registers[-1])

    def r(self, i: int) -> str:
        return hex(self.registers[i + 4])

    def __repr__(self):
        return f"Core\n" \
               f"SP={self.SP()} PC={self.PC()} SREG={self.SREG()}\n" + \
               ("\n".join(f"r{i}={self.r(i)}" for i in range(32)))


re_coredump = re.compile(r"<<<<(?P<data>([0-9a-fA-F]{2}(\s?)){37})>>>>")


def parse_core(filename: str) -> List[Core]:
    with open(filename, "r") as fp:
        content = fp.read()

    match = re_coredump.findall(content)

    cores = []
    for parsed in match:
        core = Core([int(h, 16) for h in parsed[0].split(" ")])
        cores.append(core)

    return cores


example = "<<<<55 03 51 03 81 01 0F 42 41 95 00 6E 74 6F 70 00 F1 01 FF 14 02 00 01 00 00 F1 02 FF 14 03 00 01 00 00 00 00 00>>>>"


if __name__ == "__main__":
    import sys
    if len(sys.argv) == 2:

        parse_core(sys.argv[1])
    else:
        raise Exception("argument problem")