#!/usr/bin/bash

# This script is used to calculate the size of each examples for the current commit

# go to project root
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR/..

# select the toolchain file
TOOLCHAIN_FILE=cmake/avr6-atmega2560.cmake
BUILD_TYPE=Release
BUILD_DIR=build_metrics

# build the sizes.txt
REV=$(git rev-parse HEAD)
SIZES_FILENAME=exsizes-$REV.txt
SIZES_PATH=./docs/metrics/$SIZES_FILENAME
SIZES_CONFIG_PATH=./docs/metrics/exsizes-config.txt
LATEST_SIZES_PATH=./docs/metrics/exsizes.txt

# build if path does not exist
if [ ! -d "$BUILD_DIR" ]; then
        cmake -S . -B $BUILD_DIR \
                -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE \
                -DCMAKE_GENERATOR="Ninja" \
                -DQEMU=OFF \
                -DCMAKE_BUILD_TYPE=$BUILD_TYPE
fi

ninja -C $BUILD_DIR

: > $SIZES_PATH
: > $SIZES_CONFIG_PATH

# get the size of each example
for example in $(ls -1 $BUILD_DIR/examples); do
        python3 ./scripts/parse_size_txt.py $BUILD_DIR/examples/$example/size.txt >> $SIZES_PATH
done

echo "toolchain: $TOOLCHAIN_FILE" >> $SIZES_CONFIG_PATH
echo "build type: $BUILD_TYPE" >> $SIZES_CONFIG_PATH

cp $SIZES_PATH $LATEST_SIZES_PATH

echo "$SIZES_PATH"

rm -rf $BUILD_DIR