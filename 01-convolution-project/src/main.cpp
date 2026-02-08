#include <stdio.h>

#include "conv2d.h"
#include "constants.h"
#include "utility.h"

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s <input-image-path> <output-image-path>\n", program_name);
}

int main(int argc, char **argv) {

    int res = CODE_SUCCESS;

    char *input_filename  = nullptr;
    char *output_filename = nullptr;

    float *image  = nullptr; 
    float *kernel = nullptr;
    float *output = nullptr;

    int img_height = 0, img_width = 0;
    int kernel_size = 0;
    int stride = 0;

    int out_height = 0, out_width = 0, out_size = 0;
    
    if (argc != 3) {
        print_usage(argv[0]);
        res = CODE_FAILURE_INVALID_ARG;
        goto _exit;
    }

    input_filename = argv[1];
    output_filename = argv[2];

    res = load_grayscale_image(
            input_filename, 
            image, 
            img_height, 
            img_width);

    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    kernel_size = 3;
    kernel = new float[kernel_size * kernel_size] {
        0, -1,0,
        -1, 5,-1,
        0, -1,0
    };

    stride = 1;

    out_height = (img_height - kernel_size) / stride + 1;
    out_width = (img_width - kernel_size) / stride + 1;
    out_size = out_height * out_width;
    
    Conv2DParams params;
    params.imageHeight = img_height;
    params.imageWidth  = img_width;
    params.kernelSize  = kernel_size;
    params.stride      = stride;

    output = new float[out_size];

    res = conv2d_baseline(image, kernel, params, output);

    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    res = save_float_array_as_grayscale_image(
            output_filename, 
            output, 
            out_height, 
            out_width
    );

    if (res != CODE_SUCCESS) {
        goto _exit;
    }

_exit: 
    delete[] image;
    delete[] kernel;
    delete[] output;

    return res;
}