#!/bin/bash

set -e 

rm -rf build 
mkdir build 
cd build 
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=/home/ali/Android/Sdk/ndk/26.1.10909125/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=33
make 
