cmake_minimum_required(VERSION 3.18)
project(game)

add_library(game SHARED
    native-lib.cpp
    physics.S
)

find_library(log-lib log)
target_link_libraries(game ${log-lib})

