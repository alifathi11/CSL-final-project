// #include <stdio.h>

#include "conv2d.h"
#include "constants.h"
#include "utility.h"
#include "io.h"

struct InputParams {
    int engine_mode; 
    int kernel_type;
    int kernel_size;
    std::string input_filename;
    std::string output_dir;
};

// static void print_usage(const char *program_name) {
//     fprintf(stderr, "Usage:\n");
//     fprintf(stderr, "%s <input-image-path> <output-image-path>\n", program_name);
// }

static int get_kernel(int kernel_type, int kernel_size, float*& kernel) {
    // TODO: choose kernel based on arguments

    if (
        kernel_type != KERNEL_TYPE_SHARPEN ||
        kernel_size != KERNEL_SIZE_3
    ) {
        print_err("Currently we support sharpen 3 * 3 kernel", CODE_FAILURE_NOT_SUPPORTED);
        return CODE_FAILURE_NOT_SUPPORTED;
    }

    kernel = new float[kernel_size * kernel_size] {
                0, -1, 0,
                -1, 5, -1,
                0, -1, 0
            };

    return CODE_SUCCESS;
}

static int read_interactive_input(InputParams& input_params) {

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

    char input_filename [MED_BUF_SIZE];
    char output_dir     [MED_BUF_SIZE];

    OptionEntry engine_modes[3];
    engine_modes[0].option_number = ENGINE_MODE_BASELINE;
    engine_modes[0].option_name   = ENGINE_MODE_BASELINE_STR;
    engine_modes[1].option_number = ENGINE_MODE_SIMD;
    engine_modes[1].option_name   = ENGINE_MODE_SIMD_STR;
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

    input_params.engine_mode    = engine_mode;
    input_params.kernel_type    = kernel_type;
    input_params.kernel_size    = kernel_size;
    input_params.input_filename = input_filename;
    input_params.output_dir     = output_dir;

    return CODE_SUCCESS;
}

static std::string build_output_filename(
    const InputParams& input_params
) {
    // TODO: build output filename based on input params 
    return input_params.output_dir + "/out.jpeg";
}

static int run_interactive() {

    int res = CODE_SUCCESS;

    float *image  = nullptr;
    float *kernel = nullptr;
    float *output = nullptr;

    int img_height, img_width;

    int output_height, output_width;
    int output_size;

    int stride = 1;

    InputParams input_params; 
    Conv2DParams conv2d_params;

    std::string output_filename;

    res = read_interactive_input(input_params);
    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    // TODO: currently we support just grayscale images
    res = load_grayscale_image(
                input_params.input_filename.c_str(), 
                image, 
                img_height, 
                img_width);

    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    res = get_kernel(
            input_params.kernel_type,
            input_params.kernel_size,
            kernel);

    if (res != CODE_SUCCESS) {
        goto _exit; 
    }

    output_height = (img_height - input_params.kernel_size) / stride + 1;
    output_width  = (img_width - input_params.kernel_size) / stride + 1;
    output_size   = output_height * output_width; 

    output = new float[output_size];

    conv2d_params.img_height  = img_height;
    conv2d_params.img_width   = img_width;
    conv2d_params.kernel_size = input_params.kernel_size;
    conv2d_params.stride      = stride;
 
    res = conv2d_baseline(
            image,
            kernel,
            conv2d_params,
            output);
    
    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    output_filename = build_output_filename(input_params);

    res = save_float_array_as_grayscale_image(
            output_filename.c_str(), 
            output, 
            output_height, 
            output_width);
    
    if (res != CODE_SUCCESS) {
        goto _exit; 
    }

_exit: 
    delete[] image;
    delete[] kernel;
    delete[] output;

    return res;
}

static int run_cli(int argc, char **argv) {
    // TODO: implement 

    // suppress warning
    if (argc == 1 || !argv) 
        return CODE_FAILURE_INVALID_ARG;

    return CODE_SUCCESS;
}

int main(int argc, char **argv) {

    int res = CODE_SUCCESS;

    if (argc == 1) res = run_interactive();
    else           res = run_cli(argc, argv);

    return res;
}