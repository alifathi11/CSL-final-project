#include "kernel_factory.h"
#include "constants.h"
#include "conv2d.h"
#include "utility.h"

int get_kernel(int kernel_type, int kernel_size, Kernel& kernel) {
    // TODO: choose kernel based on arguments

    if (
        kernel_type != KERNEL_TYPE_SHARPEN ||
        kernel_size != KERNEL_SIZE_3
    ) {
        print_err("Currently we support sharpen 3 * 3 kernel", CODE_FAILURE_NOT_SUPPORTED);
        return CODE_FAILURE_NOT_SUPPORTED;
    }

    kernel.data = new float[kernel_size * kernel_size] {
                0, -1, 0,
                -1, 5, -1,
                0, -1, 0
            };

    kernel.type = kernel_type;
    kernel.size = kernel_size;

    return CODE_SUCCESS;
}