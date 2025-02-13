# Generate platformio.ini environment from CMakeLists.txt for each example

import os
import re
import os.path
import glob

from typing import List

from pprint import pprint

DEFAULT_ARDUINO_SPEED = 9600

# 0: not in target_compile_definitions
# 1: in target_compile_definitions

re_target_compile_definitions = re.compile(r".*target_compile_definitions")
re_config_option = re.compile(r"^[\s\t]*(?P<option>CONFIG_([A-Z0-9_]+))=(?P<value>.*)[\s\t]*$")

def parse_cmakelists_config(cmake_path) -> List[str]:
    state = 0
    options = list()
    with open(cmake_path) as f:
        # find "target_compile_definitions"
        lines = f.readlines()
        for line in lines:
            line = line.strip("\n")
            if state == 0:
                if re_target_compile_definitions.match(line):
                    state = 1
            elif state == 1:
                m = re_config_option.match(line)
                if m:
                    options.append(m.group("option") + "=" + m.group("value"))
                if ')' in line:
                    break
    return options

def to_camel_case(snake_str):
    components = re.split(r"[-_]", snake_str)
    return "".join(x.title() for x in components)

def get_arduino_default_configuration(arduino_conf_file = "src/avrtos/avrtos_arduinoide_conf.h"):
    re_config_option = re.compile(r"^#define (?P<option>CONFIG_([A-Z0-9_]+)) (?P<value>.*)$")

    options = list()

    with open(arduino_conf_file) as f:
        lines = f.readlines()
        for line in lines:
            m = re_config_option.match(line)
            if m:
                key = m.group("option")
                val = m.group("value")

                if key == "CONFIG_SERIAL_USART_BAUDRATE":
                    options.append(f"{key}={DEFAULT_ARDUINO_SPEED}lu")
                else:
                    options.append(m.group("option") + "=" + m.group("value"))
            
    return options

arduino_examples_options = get_arduino_default_configuration()
print("Arduino default configuration:")
pprint(arduino_examples_options)

def generate_pio_env(env_name: str, example_path: str, options: List[str], isarduino: bool) -> str:
    options = "\n".join(["\t-D" + x for x in options])

    ret = f"[env:{env_name}]"

    if isarduino:
        ret += f"""
platform = atmelavr
framework = arduino
"""

    ret += f"""
build_src_filter =
    ${{env.build_src_filter}}
    +<examples/{os.path.basename(example_path)}>
"""

    if isarduino:
        ret += f"""
build_flags =
{options}

monitor_speed = {DEFAULT_ARDUINO_SPEED}
"""
    else:
        ret += f"""
build_flags =
    ${{env.build_flags}}
{options}
"""

    return ret

def generate_example_env(example_path):
    if not os.path.isdir(example_path):
        return
    
    example_dir_name: str = os.path.basename(example_path)
    
    if os.path.isfile(example_path + "/CMakeLists.txt"):
        options = parse_cmakelists_config(example_path + "/CMakeLists.txt")
        env_name = to_camel_case(example_dir_name)
        return generate_pio_env(env_name, example_path, options, False)
    elif example_dir_name.startswith("ArduinoPIO"):
        options = arduino_examples_options
        env_name = example_dir_name
        return generate_pio_env(env_name, example_path, options, True)
    else:
        print(f"\tCAN NOT generate pio env for example: {example_path}")


if __name__ == "__main__":
    print("Parsing examples...")
    envs = []
    for example in sorted(os.listdir("./examples"), key=lambda x: x.lower()):
        env = generate_example_env("./examples/" + example)

        if env:
            print("\tGenerated env: " + example)
            envs.append(env)

    print("Generating configurations for ./platformio.ini ...")

    content = "\n".join(envs)

    # open platformio.ini and seek the file after line "# examples"
    # then write the content
    with open("platformio.ini", "r+") as f:
        lines = f.readlines()
        f.seek(0)
        f.truncate(0)
        for line in lines:
            f.write(line)
            if line.strip() == "# examples":
                break
        f.write(content)
            
    print("DONE")
