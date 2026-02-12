#!/bin/bash
set -e

NDK_PATH=/home/ali/Android/Sdk/ndk/26.1.10909125

BUILD_DIR=build
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake ../../app/src/main/cpp \
    -DCMAKE_TOOLCHAIN_FILE=$NDK_PATH/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24

make -j$(nproc)


