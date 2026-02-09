#include <immintrin.h>

#include "conv2d.h"
#include "constants.h"
#include "utility.h" 

static int conv2d_sse_validation(
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

int conv2d_sse(
    const float *image,
    const float *kernel,
    const Conv2DParams& params, 
    float *output
) {
    int res = CODE_SUCCESS;

    res = conv2d_sse_validation(
            image,
            kernel, 
            params, 
            output
    );

    if (res != CODE_VALIDATION_OK) {
        print_err("Invalid arguments to function", res);
        return CODE_FAILURE;
    }

    __m128 k00 = _mm_set1_ps(kernel[0]);
    __m128 k01 = _mm_set1_ps(kernel[1]);
    __m128 k02 = _mm_set1_ps(kernel[2]);

    __m128 k10 = _mm_set1_ps(kernel[3]);
    __m128 k11 = _mm_set1_ps(kernel[4]);
    __m128 k12 = _mm_set1_ps(kernel[5]);

    __m128 k20 = _mm_set1_ps(kernel[6]);
    __m128 k21 = _mm_set1_ps(kernel[7]);
    __m128 k22 = _mm_set1_ps(kernel[8]);

    const int out_height = (params.img_height - params.kernel_size) / params.stride + 1;
    const int out_width  = (params.img_width - params.kernel_size) / params.stride + 1;

    constexpr int SSE_FLOATS = 4;

    for (int i = 0; i < out_height; i++) {
        int base_i = i * params.stride;

        int j = 0;
        for (; j <= out_width - SSE_FLOATS; j += SSE_FLOATS) {
            int base_j = j * params.stride; 

            __m128 sum = _mm_setzero_ps();

            // Row -1
            __m128 r0 = _mm_loadu_ps(&image[(base_i + 0) * params.img_width + base_j + 0]);
            __m128 r1 = _mm_loadu_ps(&image[(base_i + 0) * params.img_width + base_j + 1]);
            __m128 r2 = _mm_loadu_ps(&image[(base_i + 0) * params.img_width + base_j + 2]);

            sum = _mm_add_ps(sum, _mm_mul_ps(r0, k00));
            sum = _mm_add_ps(sum, _mm_mul_ps(r1, k01));
            sum = _mm_add_ps(sum, _mm_mul_ps(r2, k02));

            // Row 1
            r0 = _mm_loadu_ps(&image[(base_i + 1) * params.img_width + base_j + 0]);
            r1 = _mm_loadu_ps(&image[(base_i + 1) * params.img_width + base_j + 1]);
            r2 = _mm_loadu_ps(&image[(base_i + 1) * params.img_width + base_j + 2]);

            sum = _mm_add_ps(sum, _mm_mul_ps(r0, k10));
            sum = _mm_add_ps(sum, _mm_mul_ps(r1, k11));
            sum = _mm_add_ps(sum, _mm_mul_ps(r2, k12));

            // Row 2
            r0 = _mm_loadu_ps(&image[(base_i + 2) * params.img_width + base_j + 0]);
            r1 = _mm_loadu_ps(&image[(base_i + 2) * params.img_width + base_j + 1]);
            r2 = _mm_loadu_ps(&image[(base_i + 2) * params.img_width + base_j + 2]);

            sum = _mm_add_ps(sum, _mm_mul_ps(r0, k20));
            sum = _mm_add_ps(sum, _mm_mul_ps(r1, k21));
            sum = _mm_add_ps(sum, _mm_mul_ps(r2, k22));

            _mm_storeu_ps(&output[i * out_width + j], sum);
        }

        for (; j < out_width; j++) {
            int base_j = j * params.stride;
            float s = 0.f;
            for (int ky = 0; ky < 3; ky++)
                for (int kx = 0; kx < 3; kx++)
                    s += image[(base_i + ky) * params.img_width + (base_j + kx)] *
                         kernel[ky * 3 + kx];

            output[i * out_width + j] = s;
        }
    }

    return CODE_SUCCESS;
}