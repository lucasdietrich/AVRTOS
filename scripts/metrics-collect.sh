#!/usr/bin/bash

set -e

usage_help="\
        Usage: build_dir metric_path \
"

if [ $# -ne 2 ]; then
        echo "$usage_help"
        exit 1
fi

BUILD_DIR=$1
METRICS_PATH=$2

: > $METRICS_PATH

# get the size of each example
for example in $(ls -1 $BUILD_DIR/examples); do
        python3 ./scripts/parse_size_txt.py $BUILD_DIR/examples/$example/size.txt >> $METRICS_PATH
done

echo "Exported examples (build dir: $BUILD_DIR) to $METRICS_PATH"
