#!/bin/bash
set -e

NDK_PATH=/home/ali/Android/Sdk/ndk/26.1.10909125

CPP_DIR=../app/src/main/cpp
BUILD_DIR=$CPP_DIR/build

echo "== Cleaning native build =="
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR

echo "== Configuring CMake =="
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$NDK_PATH/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24

echo "== Building libgame.so =="
make -j$(nproc)

echo "== Native build finished =="

