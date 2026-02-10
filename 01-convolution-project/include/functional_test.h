#pragma once 

#include <string> 

struct FunctionalTestParams {
    int engine_mode; 
    int kernel_type;
    int kernel_size;
    std::string input_filename;
    std::string output_dir;
};

int read_functional_test_input(FunctionalTestParams& functional_test_params);
int run_functional_test(const FunctionalTestParams& functional_test_params);