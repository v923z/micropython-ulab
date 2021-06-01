#!/bin/bash

export BUILD_DIR=$(pwd)

git clone https://github.com/v923z/micropython-ulab.git ulab
git clone https://github.com/micropython/micropython.git

cd $BUILD_DIR/micropython/

git clone -b v4.0.2 --recursive https://github.com/espressif/esp-idf.git

bash
cd esp-idf
./install.sh
. ./export.sh

cd $BUILD_DIR/micropython/mpy-cross
make
cd $BUILD_DIR/micropython/ports/esp32
make submodules


echo "BOARD = GENERIC" > $BUILD_DIR/micropython/ports/esp32/makefile
echo "USER_C_MODULES = \$(BUILD_DIR)/ulab/code/micropython.cmake" >> $BUILD_DIR/micropython/ports/esp32/makefile

cd $BUILD_DIR/micropython/ports/esp32

make
