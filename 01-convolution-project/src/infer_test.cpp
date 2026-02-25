#include <iostream>

#include "infer_test.h"
#include "cnn_inference.h"
#include "constants.h"
#include "utility.h"

int read_infer_test_input(InferTestParams& infer_test_params) {

    int res = CODE_SUCCESS;

    // Default values 
    int  engine_mode_def = ENGINE_MODE_BASELINE;

    bool eval_def = false; 

    const std::string image_path_def     = "./infer_test/tensors/PNEUMONIA/person100_bacteria_475.bin";
    const std::string tensors_dir_def    = "./infer_test/tensors";
    const std::string kernel_path_def    = "./infer_test/kernel_3x3.txt";
    const std::string fc_weight_path_def = "./infer_test/fc_weight.txt";
    const std::string fc_bias_path_def   = "./infer_test/fc_bias.txt";

    // Variables
    int engine_mode;
    bool eval;
    char input[MED_BUF_SIZE];
    char kernel_path[MED_BUF_SIZE];
    char fc_weight_path[MED_BUF_SIZE];
    char fc_bias_path[MED_BUF_SIZE];

    OptionEntry engine_modes[3];
    engine_modes[0].option_number = ENGINE_MODE_BASELINE;
    engine_modes[0].option_name   = ENGINE_MODE_BASELINE_STR;
    engine_modes[1].option_number = ENGINE_MODE_SSE;
    engine_modes[1].option_name   = ENGINE_MODE_SSE_STR;
    engine_modes[2].option_number = ENGINE_MODE_AVX;
    engine_modes[2].option_name   = ENGINE_MODE_AVX_STR;

    res = read_option("Engine Mode", engine_modes, 3, stdin, &engine_mode_def, &engine_mode);
    if (res != CODE_SUCCESS) {
        print_err("Failed to read engine mode", CODE_FAILURE_READ_INPUT);
        return res;
    }

    eval = read_yes_no("Evaluate Model", stdin, eval_def);

    if (!eval) {
        res = read_param("Image Path", stdin, image_path_def.c_str(), input, sizeof(input));
        if (res != CODE_SUCCESS) {
            print_err("Failed to read image path", CODE_FAILURE_READ_INPUT);
            return res;
        }
    }
    else {
        res = read_param("Tensors Directory (test directory)", stdin, tensors_dir_def.c_str(), input, sizeof(input));
        if (res != CODE_SUCCESS) {
            print_err("Failed to read tensors directory (test directory)", CODE_FAILURE_READ_INPUT);
            return res;
        }
    }

    res = read_param("Kernel Path", stdin, kernel_path_def.c_str(), kernel_path, sizeof(kernel_path));
    if (res != CODE_SUCCESS) {
        print_err("Failed to read kernel path", CODE_FAILURE_READ_INPUT);
        return res;
    }

    res = read_param("FC Weight Path", stdin, fc_weight_path_def.c_str(), fc_weight_path, sizeof(fc_weight_path));
    if (res != CODE_SUCCESS) {
        print_err("Failed to read FC weight path", CODE_FAILURE_READ_INPUT);
        return res;
    }

    res = read_param("FC Bias Path", stdin, fc_bias_path_def.c_str(), fc_bias_path, sizeof(fc_bias_path));
    if (res != CODE_SUCCESS) {
        print_err("Failed to read FC bias path", CODE_FAILURE_READ_INPUT);
        return res;
    }


    infer_test_params.engine_mode    = engine_mode; 
    infer_test_params.input          = input;
    infer_test_params.kernel_path    = kernel_path;
    infer_test_params.fc_weight_path = fc_weight_path;
    infer_test_params.fc_bias_path   = fc_bias_path;
    infer_test_params.eval = eval;

    return res;
}

int run_infer_test(const InferTestParams& params) {

    int res;

    CNNModel model;
    res = load_model(params, model);
    if (res != CODE_SUCCESS) {
        print_err("Failed to load model", res);
        return res;
    }

    if (!params.eval) {

        int predicted;
        res = infer_single(
            params.input,
            params.engine_mode,
            model,
            predicted
        );

        if (res != CODE_SUCCESS)
            return res;

        std::cout << "Predicted Class: "
                  << predicted << std::endl;
    }
    else {

        res = evaluate_dataset(params, model);
        if (res != CODE_SUCCESS)
            return res;
    }

    delete[] model.fc_weight;
    delete[] model.fc_bias;
    delete[] model.kernel.data;

    return CODE_SUCCESS;
}