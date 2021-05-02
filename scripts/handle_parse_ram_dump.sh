set -e 
set -o pipefail

python ./handle_ram_dump.py
python ./parse_ramdump.py