cmake_minimum_required(VERSION 3.18)
project(game)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ANDROID_NDK_ROOT /home/ali/Android/Sdk/ndk/26.1.10909125)

if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${ANDROID_NDK_ROOT}/build/cmake/android.toolchain.cmake")
endif()

set(ANDROID_ABI arm64-v8a)
set(ANDROID_PLATFORM 33)

add_library(game SHARED
    native-lib.cpp
    physics.S
)

include_directories("${ANDROID_NDK_ROOT}/sysroot/usr/include")
include_directories("${ANDROID_NDK_ROOT}/sysroot/usr/include/android")

find_library(log-lib log PATHS "${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/${ANDROID_ABI}/android-${ANDROID_PLATFORM}")

target_link_libraries(game ${log-lib})

set_target_properties(game PROPERTIES
    C_STANDARD 11
    CXX_STANDARD 17
    POSITION_INDEPENDENT_CODE ON
)
