#include "kernel_factory.h"
#include "constants.h"
#include "utility.h"

#include <cmath>
#include <cstring>

static bool validate_odd_kernel(int k) {
    return (k >= 3 && k % 2 == 1);
}

static int create_sharpen(Kernel& kernel) {

    if (kernel.size != 3) {
        print_err("Sharpen kernel supports only 3x3", CODE_FAILURE_NOT_SUPPORTED);
        return CODE_FAILURE_NOT_SUPPORTED;
    }

    float data[9] = {
         0, -1,  0,
        -1,  5, -1,
         0, -1,  0
    };

    std::memcpy(kernel.data, data, 9 * sizeof(float));
    return CODE_SUCCESS;
}

static int create_sobel_x(Kernel& kernel) {

    if (kernel.size != 3) {
        print_err("Sobel X supports only 3x3", CODE_FAILURE_NOT_SUPPORTED);
        return CODE_FAILURE_NOT_SUPPORTED;
    }

    float data[9] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };

    std::memcpy(kernel.data, data, 9 * sizeof(float));
    return CODE_SUCCESS;
}

static int create_sobel_y(Kernel& kernel) {

    if (kernel.size != 3) {
        print_err("Sobel Y supports only 3x3", CODE_FAILURE_NOT_SUPPORTED);
        return CODE_FAILURE_NOT_SUPPORTED;
    }

    float data[9] = {
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    };

    std::memcpy(kernel.data, data, 9 * sizeof(float));
    return CODE_SUCCESS;
}

static int create_box_blur(Kernel& kernel) {

    if (!validate_odd_kernel(kernel.size)) {
        print_err("Box blur kernel size must be odd and >= 3", CODE_FAILURE_NOT_SUPPORTED);
        return CODE_FAILURE_NOT_SUPPORTED;
    }

    int k = kernel.size;
    float value = 1.0f / (k * k);

    for (int i = 0; i < k * k; i++) {
        kernel.data[i] = value;
    }

    return CODE_SUCCESS;
}

static int create_gaussian_blur(Kernel& kernel) {

    if (!validate_odd_kernel(kernel.size)) {
        print_err("Gaussian kernel size must be odd and >= 3", CODE_FAILURE_NOT_SUPPORTED);
        return CODE_FAILURE_NOT_SUPPORTED;
    }

    int k = kernel.size;
    int half = k / 2;

    float sigma = k / 3.0f;

    float sum = 0.0f;

    for (int y = -half; y <= half; y++) {
        for (int x = -half; x <= half; x++) {

            float exponent = -(x * x + y * y) / (2.0f * sigma * sigma);
            float value = std::exp(exponent);

            int idx = (y + half) * k + (x + half);
            kernel.data[idx] = value;

            sum += value;
        }
    }

    for (int i = 0; i < k * k; i++) {
        kernel.data[i] /= sum;
    }

    return CODE_SUCCESS;
}

int get_kernel(int kernel_type, int kernel_size, Kernel& kernel) {

    if (!validate_odd_kernel(kernel_size)) {
        print_err("Kernel size must be odd and >= 3", CODE_FAILURE_NOT_SUPPORTED);
        return CODE_FAILURE_NOT_SUPPORTED;
    }

    kernel.size = kernel_size;
    kernel.type = kernel_type;
    kernel.data = new float[kernel_size * kernel_size];

    int status = CODE_SUCCESS;

    switch (kernel_type) {

        case KERNEL_TYPE_SHARPEN:
            status = create_sharpen(kernel);
            break;

        case KERNEL_TYPE_SOBEL_X:
            status = create_sobel_x(kernel);
            break;

        case KERNEL_TYPE_SOBEL_Y:
            status = create_sobel_y(kernel);
            break;

        case KERNEL_TYPE_BOX_BLUR:
            status = create_box_blur(kernel);
            break;

        case KERNEL_TYPE_GAUSSIAN_BLUR:
            status = create_gaussian_blur(kernel);
            break;

        default:
            print_err("Unsupported kernel type", CODE_FAILURE_NOT_SUPPORTED);
            status = CODE_FAILURE_NOT_SUPPORTED;
            break;
    }

    if (status != CODE_SUCCESS) {
        delete[] kernel.data;
        kernel.data = nullptr;
    }

    return status;
}
