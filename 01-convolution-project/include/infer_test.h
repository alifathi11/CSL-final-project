#pragma once

#include <string>

struct InferTestParams {
    int engine_mode;
    
    std::string input;
    std::string kernel_path;
    std::string fc_weight_path;
    std::string fc_bias_path;

    bool eval = false;
};

int read_infer_test_input(InferTestParams& infer_test_params);
int run_infer_test(const InferTestParams& infer_test_params);