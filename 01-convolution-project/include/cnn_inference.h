#pragma once 

#include "conv2d.h"
#include "infer_test.h"

struct CNNModel {
    Kernel kernel;
    const float* fc_weight = nullptr;
    const float* fc_bias   = nullptr;
    int in_features = 0;
    int out_features = 2;
};

int load_model(
    const InferTestParams& params,
    CNNModel& model
);

int infer_single(
    const std::string& image_path,
    int engine_mode,
    const CNNModel& model,
    int& predicted_class
);

int evaluate_dataset(
    const InferTestParams& params,
    const CNNModel& model
);