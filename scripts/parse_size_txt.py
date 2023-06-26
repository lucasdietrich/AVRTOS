import os
import re
import sys

def parse_memory_usage(file_path):
    """Parse a file of the following format and synthesize the result into a 
    single line printed to stdout.
        
        AVR Memory Usage
        ----------------
        Device: atmega2560

        Program:    5566 bytes (2.1% Full)
        (.text + .data + .bootloader)

        Data:       1836 bytes (22.4% Full)
        (.data + .bss + .noinit)

    Args:
        file_path (string): file to path
    """
    if not os.path.isfile(file_path):
        return

    with open(file_path, 'r') as file:
        content = file.read()

    example = os.path.basename(os.path.dirname(file_path))

    device_match = re.search(r'Device:\s+(\w+)', content)
    program_match = re.search(r'Program:\s+(?P<flash>\d+)\s+bytes\s+\((?P<flash_percent>\d+(\.\d+)?)% Full\)', content)
    data_match = re.search(r'Data:\s+(?P<ram>\d+)\s+bytes\s+\((?P<ram_percent>\d+(\.\d+)?)% Full\)', content)

    if device_match and program_match and data_match:
        device = device_match.group(1)
        program_size = program_match.group("flash")
        program_size_percentage = program_match.group("flash_percent")
        data_size = data_match.group("ram")
        data_size_percentage = data_match.group("ram_percent")

        output = \
                f"[ {device} ] ".ljust(16) + \
                f"{example} ".ljust(40) + \
                f"flash {program_size} B ".ljust(14) + \
                f"({program_size_percentage} %) ".ljust(10) + \
                f"ram {data_size} B ".ljust(12) + \
                f"({data_size_percentage} %)".ljust(10)
        return output

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f"Usage: python {sys.argv[0]} <file_path>", file=sys.stderr)
        sys.exit(1)

    file_path = sys.argv[1]
    result = parse_memory_usage(file_path)

    if result:
        print(result)
    else:
        print(f"Invalid file path or format: {file_path}", file=sys.stderr)