# Generate platformio.ini environment from CMakeLists.txt for each example

import os
import re
import os.path

from typing import List

from pprint import pprint

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

def generate_pio_env(example_path):
    if not os.path.isdir(example_path):
        return
    
    if not os.path.isfile(example_path + "/CMakeLists.txt"):
        return
    
    options = parse_cmakelists_config(example_path + "/CMakeLists.txt")
    options = "\n".join(["\t-D" + x for x in options])

    env_name = to_camel_case(os.path.basename(example_path))

    ret = f"""
[env:{env_name}]
build_src_filter =
    ${{env.build_src_filter}}
    +<examples/{os.path.basename(example_path)}>

build_flags =
    ${{env.build_flags}}
{options}
"""

    return ret

if __name__ == "__main__":
    for example in os.listdir("./examples"):
        env = generate_pio_env("./examples/" + example)

        if env:
            print(env)
