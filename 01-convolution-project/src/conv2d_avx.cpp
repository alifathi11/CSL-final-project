#include "constants.h"
#include "conv2d.h"

int conv2d_avx(
    const float *image,
    const float *kernel,
    const Conv2DParams& params, 
    float *output
) {
    if (!image || !kernel || !output) return CODE_FAILURE_INVALID_ARG;
    if (params.kernel_size <= 0 || params.img_height <= 0 || params.img_width <= 0) return CODE_FAILURE_INVALID_ARG;

    return CODE_SUCCESS;
}