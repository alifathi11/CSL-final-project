#include <chrono>

#include "functional_test.h"
#include "conv2d.h"
#include "io.h"
#include "kernel_factory.h"
#include "utility.h"
#include "constants.h"

static int read_functional_test_input(FunctionalTestParams& functional_test_params) {

    int res = CODE_SUCCESS;

    // Default Values 
    int engine_mode_def = ENGINE_MODE_BASELINE;
    int kernel_type_def = KERNEL_TYPE_SHARPEN;
    int kernel_size_def = KERNEL_SIZE_3;

    const std::string input_filename_def = "./images/grayscale/01.jpg";
    const std::string output_dir_def     = "./images/output";

    // Variables 
    int engine_mode;
    int kernel_type;
    int kernel_size;

    char input_filename[MED_BUF_SIZE];
    char output_dir    [MED_BUF_SIZE];

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

    OptionEntry kernel_types[3];
    kernel_types[0].option_number = KERNEL_TYPE_SHARPEN;
    kernel_types[0].option_name   = KERNEL_TYPE_SHARPEN_STR;
    kernel_types[1].option_number = KERNEL_TYPE_BLUR;
    kernel_types[1].option_name   = KERNEL_TYPE_BLUR_STR;
    kernel_types[2].option_number = KERNEL_TYPE_EDGE;
    kernel_types[2].option_name   = KERNEL_TYPE_EDGE_STR;

    res = read_option("Kernel Type", kernel_types, 3, stdin, &kernel_type_def, &kernel_type);
    if (res != CODE_SUCCESS) {
        print_err("Failed to read kernel type", CODE_FAILURE_READ_INPUT);
        return res;
    }

    OptionEntry kernel_sizes[3];
    kernel_sizes[0].option_number = KERNEL_SIZE_3;
    kernel_sizes[0].option_name   = KERNEL_SIZE_3_STR;
    kernel_sizes[1].option_number = KERNEL_SIZE_5;
    kernel_sizes[1].option_name   = KERNEL_SIZE_5_STR;
    kernel_sizes[2].option_number = KERNEL_SIZE_7;
    kernel_sizes[2].option_name   = KERNEL_SIZE_7_STR;

    res = read_option("Kernel Size", kernel_sizes, 3, stdin, &kernel_size_def, &kernel_size);
    if (res != CODE_SUCCESS) {
        print_err("Failed to read kernel size", CODE_FAILURE_READ_INPUT);
        return res;
    }

    res = read_param("Image Path", stdin, input_filename_def.c_str(), input_filename, sizeof(input_filename));
    if (res != CODE_SUCCESS) {
        print_err("Failed to read image path", CODE_FAILURE_READ_INPUT);
        return res;
    }

    res = read_param("Output Directory", stdin, output_dir_def.c_str(), output_dir, sizeof(output_dir));
    if (res != CODE_SUCCESS) {
        print_err("Failed to read output directory", CODE_FAILURE_READ_INPUT);
        return res;
    }

    functional_test_params.engine_mode    = engine_mode;
    functional_test_params.kernel_type    = kernel_type;
    functional_test_params.kernel_size    = kernel_size;
    functional_test_params.input_filename = input_filename;
    functional_test_params.output_dir     = output_dir;

    return res;
}

static int load_image(
    const std::string& image_filename,
    Image& image
) {
    int res = CODE_SUCCESS;

    // TODO: currently we support just grayscale images
    res = load_grayscale_image(
                image_filename.c_str(),
                image);

    if (res != CODE_SUCCESS) {
        std::string err_msg = "Failed to load image: " + image_filename;
        print_err(err_msg.c_str(), res);
        return CODE_FAILURE;
    }

    return res;
}

static int save_image(
    const std::string& dir, 
    const Image& image
) {
    // TODO: use a function to build output filename 

    int res = CODE_SUCCESS;

    char output_filename[MED_BUF_SIZE];

    snprintf(output_filename, sizeof(output_filename), "%s/out.jpeg", dir.c_str());

    res = save_float_array_as_grayscale_image(
            output_filename, 
            image);
    
    if (res != CODE_SUCCESS) {
        std::string err_msg = "Failed to save image: " + std::string(output_filename);
        print_err(err_msg.c_str(), res);
        return CODE_FAILURE;
    }

    return res;
}

int run_functional_test() {

    int res = CODE_SUCCESS;

    Image input_img, output_img;
    Kernel kernel;

    int stride = 1;

    FunctionalTestParams functional_test_params; 
    Conv2DParams conv2d_params;

    std::chrono::time_point<std::chrono::high_resolution_clock> t0, t1;
    std::chrono::duration<double, std::milli> elapsed;

    res = read_functional_test_input(functional_test_params);
    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    // TODO: currently we support just grayscale images
    res = load_image(
                functional_test_params.input_filename, 
                input_img);

    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    res = get_kernel(
            functional_test_params.kernel_type,
            functional_test_params.kernel_size,
            kernel);

    if (res != CODE_SUCCESS) {
        goto _exit; 
    }

    conv2d_params.image  = input_img;
    conv2d_params.kernel = kernel;
    conv2d_params.stride = stride;

    t0 = std::chrono::high_resolution_clock::now();

    res = conv2d(
            functional_test_params.engine_mode,
            conv2d_params,
            output_img);

    t1 = std::chrono::high_resolution_clock::now();
    
    elapsed = t1 - t0; 

    res = save_image(functional_test_params.output_dir, output_img);
    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    print_benchmark(functional_test_params.engine_mode, elapsed.count());

_exit: 
    delete[] input_img.data;
    delete[] output_img.data;
    delete[] kernel.data;

    return res;
}