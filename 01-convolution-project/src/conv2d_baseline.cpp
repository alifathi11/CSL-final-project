#include "conv2d.h"
#include "constants.h"
#include "utility.h"

static bool is_valid_kernel_size(int kernel_size) {
    return (
        kernel_size == KERNEL_SIZE_3 ||
        kernel_size == KERNEL_SIZE_5
    );
}

static bool is_valid_image_size(int img_height, int img_width) {
    return (
        img_height > 0 && img_width > 0
    );
}

static bool is_valid_stride(int stride) {
    return (
        stride >= 1
    );
}

static bool is_valid_conv2d_params(const Conv2DParams& params) {
    if (
        !is_valid_kernel_size(params.kernel_size) ||
        !is_valid_image_size(params.img_height, params.img_width) ||
        !is_valid_stride(params.stride)
    ) {
        return false;
    }

    return true;
}

static int conv2d_baseline_validation(
    const float *image, 
    const float *kernel, 
    const Conv2DParams& params, 
    float *output
) {
    if (!image || !kernel || !output) {
        return CODE_FAILURE_INVALID_ARG;
    }

    if (!is_valid_conv2d_params(params)) {
        return CODE_FAILURE_INVALID_ARG;
    }

    return CODE_VALIDATION_OK;
}

int conv2d_baseline(
    const float *image, 
    const float *kernel, 
    const Conv2DParams& params, 
    float *output
) {

    int res = CODE_SUCCESS;

    res = conv2d_baseline_validation(
            image,
            kernel, 
            params, 
            output
    );

    if (res != CODE_VALIDATION_OK) {
        print_err("Invalid arguments to function", res);
        return CODE_FAILURE;
    }

    const int outHeight = (params.img_height - params.kernel_size) / params.stride + 1;
    const int outWidth  = (params.img_width - params.kernel_size) / params.stride + 1;

    for (int i = 0; i < outHeight; i++) {
        for (int j = 0; j < outWidth; j++) {

            float sum = 0.0f;

            const int base_i = i * params.stride;
            const int base_j = j * params.stride;

            for (int u = 0; u < params.kernel_size; u++) {
                
                const int img_row = (base_i + u) * params.img_width;
                const int ker_row = u * params.kernel_size;

                for (int v = 0; v < params.kernel_size; v++) {
                    int img_idx = img_row + (base_j + v);
                    int ker_idx = ker_row + v;

                    sum += image[img_idx] * kernel[ker_idx];
                }
            }

            int out_idx = i * outWidth + j;
            output[out_idx] = sum; 
        }
    }

    return CODE_SUCCESS;
}