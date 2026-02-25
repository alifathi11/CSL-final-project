#include <cmath>
#include <cstring>
#include <iostream>
#include <filesystem>

#include "cnn_inference.h"
#include "infer_test.h"
#include "constants.h"
#include "io.h"
#include "conv2d.h"
#include "utility.h"

static inline void relu(Image& img) {
    int total = img.height * img.width * img.channels;
    for (int i = 0; i < total; i++) {
        if (img.data[i] < 0.0f) {
            img.data[i] = 0.0f;
        }
    }
}

static inline void linear(
    const float* input, 
    const float* weight,
    const float* bias, 
    float *output, 
    int in_features, 
    int out_features
) {
    for (int o = 0; o < out_features; o++) {
        float sum = bias[o];

        for (int i = 0; i < in_features; i++) {
            sum += input[i] * weight[o * in_features + i];
        }

        output[o] = sum;
    }
}

static inline float* flatten(const Image& img) {
    int total = img.height * img.width * img.channels;
    float* flat = new float[total];
    
    std::memcpy(flat, img.data, total * sizeof(float));

    return flat;
}

int load_model(
    const InferTestParams& params,
    CNNModel& model
) {
    int res;

    res = load_kernel_from_file(params.kernel_path.c_str(), model.kernel);
    if (res != CODE_SUCCESS) return res;

    // Compute conv output size using dummy image size
    int img_h = 64;
    int img_w = 64;

    int conv_out_h = img_h - model.kernel.size + 1;
    int conv_out_w = img_w - model.kernel.size + 1;

    model.in_features = conv_out_h * conv_out_w;

    model.fc_weight = load_matrix(
        params.fc_weight_path.c_str(),
        model.out_features,
        model.in_features
    );

    model.fc_bias = load_vector(
        params.fc_bias_path.c_str(),
        model.out_features
    );

    if (!model.fc_weight || !model.fc_bias)
        return CODE_FAILURE;

    return CODE_SUCCESS;
}

int choose_class(
    int engine_mode,
    const Conv2DParams& conv2d_params, 
    const float* fc_weight,
    const float* fc_bias,
    int in_features, 
    int out_features,
    int& predicted_class
) {
    int res = CODE_SUCCESS;

    Image conv_out;
    res = conv2d_channels(engine_mode, conv2d_params, conv_out);
    if (res != CODE_SUCCESS) {
        print_err("Failed to run conv2d_channels", res);
        return res;
    }

    relu(conv_out);

    float* flat = flatten(conv_out);

    float output[2];

    linear(flat, fc_weight, fc_bias, output, in_features, out_features);

    predicted_class = (output[1] > output[0]) ? 1 : 0;

    delete[] flat;
    delete[] conv_out.data;

    return CODE_SUCCESS;
}

int infer_single(
    const std::string& image_path,
    int engine_mode,
    const CNNModel& model,
    int& predicted_class
) {
    int res;

    Image img;
    res = load_tensor(image_path, img);
    if (res != CODE_SUCCESS) {
        print_err("Failed to load tensor", res);
        return res;
    }

    Conv2DParams params;
    params.image  = img;
    params.kernel = model.kernel;
    params.stride = 1;

    res = choose_class(
        engine_mode,
        params,
        model.fc_weight,
        model.fc_bias,
        model.in_features,
        model.out_features,
        predicted_class
    );

    if (res != CODE_SUCCESS) {
        print_err("Failed to choose class", res);
        return res;
    }
    
    delete[] img.data;

    return res;
}

int evaluate_dataset(
    const InferTestParams& params,
    const CNNModel& model
) {
    int total = 0;
    int correct = 0;

    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& class_dir : std::filesystem::directory_iterator(params.input)) {

        if (!class_dir.is_directory())
            continue;

        std::string label_name = class_dir.path().filename().string();

        int label;
        if (label_name == "NORMAL") label = 0;
        else if (label_name == "PNEUMONIA") label = 1;
        else continue;

        for (const auto& file : std::filesystem::directory_iterator(class_dir.path())) {

            if (file.path().extension() != ".bin")
                continue;

            int predicted;
            int res = infer_single(
                file.path().string(),
                params.engine_mode,
                model,
                predicted
            );

            if (res != CODE_SUCCESS)
                return res;

            if (predicted == label)
                correct++;

            total++;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    double seconds =
        std::chrono::duration<double>(end - start).count();

    std::cout << "\n===== Evaluation Results =====\n";
    std::cout << "Samples: " << total << std::endl;
    std::cout << "Accuracy: "
              << (100.0 * correct / total) << "%\n";
    std::cout << "Total Time: " << seconds << " sec\n";
    std::cout << "Avg Time / Sample: "
              << (seconds / total) * 1000 << " ms\n";

    return CODE_SUCCESS;
}
