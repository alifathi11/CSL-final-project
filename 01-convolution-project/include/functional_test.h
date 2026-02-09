#pragma once 

#include <string> 

struct FunctionalTestParams {
    int engine_mode; 
    int kernel_type;
    int kernel_size;
    std::string input_filename;
    std::string output_dir;
};

int run_functional_test();