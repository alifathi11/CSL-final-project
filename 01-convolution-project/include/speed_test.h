#pragma once 

#include <string> 

struct SpeedTestParams {
    int engine_mode;
    int kernel_type;
    int kernel_size;
    std::string input_dir;
    std::string output_dir; 

    bool save_output = false;
};

int read_speed_test_params(SpeedTestParams& speed_test_params);
int run_speed_test(const SpeedTestParams& speed_test_params);