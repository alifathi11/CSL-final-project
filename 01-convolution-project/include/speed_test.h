#pragma once 

#include <string> 

struct SpeedTestParams {
    int engine_mode;
    int kernel_type;
    int kernel_size;
    std::string input_dir;
    std::string output_dir; 
};

int run_speed_test();