import os
import re
import glob
from colorama import Fore, Style

if __name__ == "__main__":
    print("Generating Arduino samples from ArduinoPIO ...")

    # Match "ArduinoPIO*" and store rest (*) in group "name"
    re_arduino_pio = re.compile(r"ArduinoPIO(?P<name>.*)")

    for example in os.listdir("./examples"):
        m = re_arduino_pio.match(example)
        if m:
            sample_name = m.group("name")
            arduino_sample_name = "Arduino" + sample_name
            
            # Create folder "./examples/${arduino_sample_name}"
            os.makedirs(os.path.join("./examples", arduino_sample_name), exist_ok=True)

            found_main = False

            for file in glob.glob(os.path.join("./examples", example, "*.cpp")):
                basename = os.path.basename(file)
                if basename in [
                    sample_name + ".cpp",
                    sample_name + ".c",
                ]:
                    found_main = True


                with open(file, "r") as f:
                    content = f.read()
                
                # re match multiline "#include <Arduino.h>" + following line if empty
                re_include_arduino = re.compile(r"#include <Arduino.h>\s*\n(\s*\n)?", re.MULTILINE)

                content = re_include_arduino.sub("", content)

                with open(os.path.join("./examples", arduino_sample_name, arduino_sample_name + ".ino"), "w") as f:
                    f.write(content)

                print(f"Generated {arduino_sample_name}/{arduino_sample_name}.ino")

            if not found_main:
                print(f"{Fore.RED}No main file found for {arduino_sample_name}{Style.RESET_ALL}")