import serial
import datetime

import os

# todo handle several ramdump in the same run

LOCATION_DIR = "./res"
COM_PORT = "COM3"
BAUDRATE = 115200

FILENAME = "ramdump.txt" # set None to have a new file every time

if FILENAME is None:
    now = datetime.datetime.now().strftime("%Y%m%d-%H%M%S");

    filename = f"ramdump-{now}.txt"
else:
    filename = FILENAME

filepath = os.path.abspath(os.path.join(LOCATION_DIR, filename))

ser = serial.Serial(COM_PORT, BAUDRATE)

state = 0
boundary = b'============\n'

with open(filepath, "wb+") as fp:
    while state < 2:
        content = ser.read_until()
        
        if content == boundary:
            state += 1
            print(f"Baudary found (state = {state})")
        elif state == 1:
            fp.write(content)
    
print(f"flash dump complete : {filepath}")