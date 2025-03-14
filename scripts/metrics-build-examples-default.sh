
#!/usr/bin/bash

# This script is used to calculate the size of each examples for the current commit

# go to project root
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR/..

# select the toolchain file
TOOLCHAIN_FILE=${TOOLCHAIN_FILE:-"cmake/avr6-atmega2560.cmake"}
BUILD_TYPE=${BUILD_TYPE:-"Release"}
BUILD_DIR=${BUILD_DIR:-"build_metrics"}
METRICS_PATH=${METRICS_PATH:-"docs/metrics"}

REV=$(git rev-parse HEAD)
METRICS_FILE_REV_PATH=$METRICS_PATH/exsizes-$(git rev-parse HEAD).txt
METRICS_FILE_PATH=$METRICS_PATH/exsizes.txt
METRICS_CONFIG_FILE_PATH=$METRICS_PATH/exsizes-config.txt

echo "Toolchain: $TOOLCHAIN_FILE, Build type: $BUILD_TYPE, Build dir: $BUILD_DIR"

cmake -S . -B $BUILD_DIR \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE \
        -DCMAKE_GENERATOR="Ninja" \
        -DQEMU=OFF \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE
ninja -C $BUILD_DIR

: > $METRICS_CONFIG_FILE_PATH

bash ./scripts/metrics-collect.sh $BUILD_DIR $METRICS_FILE_PATH

echo "toolchain: $TOOLCHAIN_FILE" >> $METRICS_CONFIG_FILE_PATH
echo "build type: $BUILD_TYPE" >> $METRICS_CONFIG_FILE_PATH
echo "avr-gcc version: $(avr-gcc --version | head -n 1)" >> $METRICS_CONFIG_FILE_PATH

cp $METRICS_FILE_PATH $METRICS_FILE_REV_PATH

echo "$METRICS_FILE_PATH"

rm -rf $BUILD_DIR