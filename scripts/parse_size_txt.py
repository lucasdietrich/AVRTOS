import os
import re
import sys

def get_mcu_memory_sizes(mcu):
    """Returns flash and RAM sizes for known MCUs."""
    memory_sizes = {
        "atmega328p": {"flash": 32256, "ram": 2048},
        "atmega2560": {"flash": 253952, "ram": 8192}
    }
    return memory_sizes.get(mcu.lower(), None)

def detect_format(content):
    """Detects the format used in the memory usage file."""
    if re.search(r'Device:\s+(\w+)', content) and re.search(r'Program:\s+', content):
        return "avr"
    elif re.search(r'\s*\d+\s+\d+\s+\d+\s+\d+\s+[0-9a-fA-F]+', content):
        return "berkeley"
    return None

def parse_avr_format(content):
    """Parses the AVR legacy format."""
    device_match = re.search(r'Device:\s+(\w+)', content)
    program_match = re.search(r'Program:\s+(?P<flash>\d+)\s+bytes\s+\((?P<flash_percent>\d+(\.\d+)?)% Full\)', content)
    data_match = re.search(r'Data:\s+(?P<ram>\d+)\s+bytes\s+\((?P<ram_percent>\d+(\.\d+)?)% Full\)', content)
    
    if device_match and program_match and data_match:
        return {
            "device": device_match.group(1),
            "program_size": int(program_match.group("flash")),
            "program_size_percentage": float(program_match.group("flash_percent")),
            "data_size": int(data_match.group("ram")),
            "data_size_percentage": float(data_match.group("ram_percent"))
        }
    return None

def parse_berkeley_format(content, mcu_sizes, mcu):
    """Parses the Berkeley format."""
    size_match = re.search(r'\s*(?P<text>\d+)\s+(?P<data>\d+)\s+(?P<bss>\d+)\s+(?P<dec>\d+)\s+(?P<hex>[0-9a-fA-F]+)', content)
    
    if size_match:
        text_size = int(size_match.group("text"))
        data_size = int(size_match.group("data"))
        bss_size = int(size_match.group("bss"))
        
        program_size = text_size + data_size
        program_size_percentage = (program_size / mcu_sizes["flash"]) * 100
        
        ram_usage = data_size + bss_size
        data_size_percentage = (ram_usage / mcu_sizes["ram"]) * 100
        
        return {
            "device": mcu,
            "program_size": program_size,
            "program_size_percentage": program_size_percentage,
            "data_size": ram_usage,
            "data_size_percentage": data_size_percentage
        }
    return None

def parse_memory_usage(file_path, mcu):
    """Parses the memory usage file and returns formatted output."""
    if not os.path.isfile(file_path):
        print(f"Error: File not found - {file_path}", file=sys.stderr)
        return None
    
    mcu_sizes = get_mcu_memory_sizes(mcu)
    if not mcu_sizes:
        print(f"Error: Unsupported MCU - {mcu}", file=sys.stderr)
        return None

    with open(file_path, 'r') as file:
        content = file.read()
    
    format_type = detect_format(content)
    parsed_data = None
    
    if format_type == "avr":
        parsed_data = parse_avr_format(content)
    elif format_type == "berkeley":
        parsed_data = parse_berkeley_format(content, mcu_sizes, mcu)
    else:
        print(f"Error: Unrecognized format in {file_path}", file=sys.stderr)
        return None
    
    if parsed_data:
        output = (
            f"[ {parsed_data['device']} ] ".ljust(16) +
            f"{os.path.basename(os.path.dirname(file_path))} ".ljust(40) +
            f"flash {parsed_data['program_size']} B ".ljust(14) +
            f"({parsed_data['program_size_percentage']:.1f} %) ".ljust(10) +
            f"ram {parsed_data['data_size']} B ".ljust(12) +
            f"({parsed_data['data_size_percentage']:.1f} %)"
        )
        return output
    return None

if __name__ == '__main__':
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print(f"Usage: python {sys.argv[0]} <file_path> [mcu]", file=sys.stderr)
        sys.exit(1)
    
    file_path = sys.argv[1]
    mcu = sys.argv[2] if len(sys.argv) == 3 else "atmega2560"
    
    result = parse_memory_usage(file_path, mcu)
    
    if result:
        print(result)
