import glob
import os

from parse_coredump_hex import parse_core
from parse_ramdump import parse_ram

list_of_files = glob.glob('../*.log') # * means all if need specific format then *.csv

print(list_of_files)

if list_of_files:
    latest_file = max(list_of_files, key=os.path.getmtime)

    # parse_ram(latest_file, latest_file + ".ramdump.txt")
    print(parse_core(latest_file))