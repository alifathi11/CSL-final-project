#include "kernel_factory.h"
#include "constants.h"
#include "conv2d.h"
#include "utility.h"

int get_kernel(int kernel_type, int kernel_size, Kernel& kernel) {

    if (kernel_size != KERNEL_SIZE_3) {
        print_err("Only 3x3 kernels are supported", CODE_FAILURE_NOT_SUPPORTED);
        return CODE_FAILURE_NOT_SUPPORTED;
    }

    kernel.data = new float[kernel_size * kernel_size];
    kernel.type = kernel_type;
    kernel.size = kernel_size;

    switch (kernel_type) {

        case KERNEL_TYPE_SHARPEN:
            kernel.data[0] =  0; kernel.data[1] = -1; kernel.data[2] =  0;
            kernel.data[3] = -1; kernel.data[4] =  5; kernel.data[5] = -1;
            kernel.data[6] =  0; kernel.data[7] = -1; kernel.data[8] =  0;
            break;

        case KERNEL_TYPE_SOBEL_X:
            kernel.data[0] = -1; kernel.data[1] =  0; kernel.data[2] =  1;
            kernel.data[3] = -2; kernel.data[4] =  0; kernel.data[5] =  2;
            kernel.data[6] = -1; kernel.data[7] =  0; kernel.data[8] =  1;
            break;

        case KERNEL_TYPE_SOBEL_Y:
            kernel.data[0] = -1; kernel.data[1] = -2; kernel.data[2] = -1;
            kernel.data[3] =  0; kernel.data[4] =  0; kernel.data[5] =  0;
            kernel.data[6] =  1; kernel.data[7] =  2; kernel.data[8] =  1;
            break;

        default:
            print_err("Unsupported kernel type", CODE_FAILURE_NOT_SUPPORTED);
            delete[] kernel.data;
            return CODE_FAILURE_NOT_SUPPORTED;
    }

    return CODE_SUCCESS;
}
