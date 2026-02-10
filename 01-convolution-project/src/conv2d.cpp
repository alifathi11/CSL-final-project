#include <cstring>
#include <immintrin.h>
#include <string>

#include "conv2d.h"
#include "constants.h"
#include "utility.h"

static int conv2d_baseline(
    const Conv2DParams& params, 
    Image& output
);

static int conv2d_sse(
    const Conv2DParams& params, 
    Image& output
);

static int conv2d_avx(
    const Conv2DParams& params, 
    Image& output
);

static bool is_valid_engine_mode(int engine_mode) {
    return (
        engine_mode == ENGINE_MODE_BASELINE ||
        engine_mode == ENGINE_MODE_SSE      || 
        engine_mode == ENGINE_MODE_AVX
    );
}

static bool is_valid_kernel(const Kernel& kernel) {
    if (
        !kernel.data ||
        !(kernel.size == KERNEL_SIZE_3 || kernel.size == KERNEL_SIZE_5 || kernel.size == KERNEL_SIZE_7) ||
        !(kernel.type == KERNEL_TYPE_SHARPEN || kernel.type == KERNEL_TYPE_BLUR || kernel.type == KERNEL_TYPE_EDGE)
    ) {
        return false;
    }

    return true;
}

static bool is_valid_image(const Image& image ) {
    if (
        !image.data ||
        !(image.height > 0 && image.width > 0)
    ) {
        return false;
    }

    return true;
}

static bool is_valid_stride(int stride) {
    // Currently we just support stride 1
    return (
        stride == 1
    );
}

static bool is_valid_conv2d_params(const Conv2DParams& params) {
    if (
        !is_valid_kernel(params.kernel) ||
        !is_valid_image(params.image)   || 
        !is_valid_stride(params.stride)
    ) {
        return false;
    }

    return true;
}

static int conv2d_validation(
    int engine_mode, 
    const Conv2DParams& params
) {
    if (!is_valid_engine_mode(engine_mode)) {
        return CODE_FAILURE_INVALID_ARG;
    }

    if (!is_valid_conv2d_params(params)) {
        return CODE_FAILURE_INVALID_ARG;
    }

    return CODE_VALIDATION_OK;
}

static float* channel_ptr(const Image& img, int c) {

    if (c >= img.channels) 
        return nullptr;

    return img.data + c * img.height * img.width;
}

int conv2d_channels(
    int engine_mode, 
    const Conv2DParams& params, 
    Image& output
) {

    int res = CODE_SUCCESS;

    Image image   = params.image;
    Kernel kernel = params.kernel;

    int out_height = (image.height - kernel.size) / params.stride + 1;
    int out_width  = (image.width - kernel.size) / params.stride + 1;
    
    output.height   = out_height;
    output.width    = out_width;
    output.channels = image.channels;
    output.data     = new float[output.height * output.width * output.channels];

    for (int c = 0; c < output.channels; c++) {
        Conv2DParams ch_params = params;
        ch_params.image.data = channel_ptr(image, c);

        Image ch_out;
        res = conv2d(engine_mode, ch_params, ch_out);

        if (res != CODE_SUCCESS) {
            std::string err_msg = "Operation failed on channel " + std::to_string(c);
            print_err(err_msg.c_str(), res);
            return res;
        }

        std::memcpy( 
            channel_ptr(output, c),
            ch_out.data,
            output.height * output.width * sizeof(float)
        );

        delete[] ch_out.data;
    }

    return res;
}

int conv2d(
    int engine_mode, 
    const Conv2DParams& params, 
    Image& output
) {
    int res = CODE_SUCCESS;
    
    res = conv2d_validation(
            engine_mode,  
            params);

    if (res != CODE_VALIDATION_OK) {
        print_err("Invalid arguments to function", res);
        return CODE_FAILURE_INVALID_ARG;
    }

    switch(engine_mode) {
        case ENGINE_MODE_BASELINE: 
            res = conv2d_baseline(
                    params, 
                    output);
            break;
        
        case ENGINE_MODE_SSE:
            res = conv2d_sse(
                    params, 
                    output);
            break;
    
        case ENGINE_MODE_AVX:
            res = conv2d_avx(
                    params, 
                    output);
            break;
            
        default: res = CODE_FAILURE;
    }

    return res;
}

static int conv2d_baseline(
    const Conv2DParams& params, 
    Image& output
) {

    int res = CODE_SUCCESS;

    Image image = params.image;
    Kernel kernel = params.kernel;

    int out_height = (image.height - kernel.size) / params.stride + 1;
    int out_width  = (image.width - kernel.size) / params.stride + 1;
    int out_size   = out_height * out_width;

    output.height = out_height;
    output.width  = out_width;
    output.data   = new float[out_size];

    for (int i = 0; i < out_height; i++) {
        for (int j = 0; j < out_width; j++) {

            float sum = 0.0f;

            int base_i = i * params.stride;
            int base_j = j * params.stride;

            for (int u = 0; u < kernel.size; u++) {
                
                int img_row = (base_i + u) * image.width;
                int ker_row = u * kernel.size;

                for (int v = 0; v < kernel.size; v++) {
                    int img_idx = img_row + (base_j + v);
                    int ker_idx = ker_row + v;

                    sum += image.data[img_idx] * kernel.data[ker_idx];
                }
            }

            int out_idx = i * out_width + j;
            output.data[out_idx] = sum; 
        }
    }

    return res;
}

static int conv2d_sse(
    const Conv2DParams& params, 
    Image& output
) {
    int res = CODE_SUCCESS;

    Image image = params.image;
    Kernel kernel = params.kernel;

    int out_height = (image.height - kernel.size) / params.stride + 1;
    int out_width  = (image.width - kernel.size) / params.stride + 1;
    int out_size   = out_height * out_width;

    output.height = out_height;
    output.width  = out_width;
    output.data   = new float[out_size];

    __m128 k00 = _mm_set1_ps(kernel.data[0]);
    __m128 k01 = _mm_set1_ps(kernel.data[1]);
    __m128 k02 = _mm_set1_ps(kernel.data[2]);

    __m128 k10 = _mm_set1_ps(kernel.data[3]);
    __m128 k11 = _mm_set1_ps(kernel.data[4]);
    __m128 k12 = _mm_set1_ps(kernel.data[5]);

    __m128 k20 = _mm_set1_ps(kernel.data[6]);
    __m128 k21 = _mm_set1_ps(kernel.data[7]);
    __m128 k22 = _mm_set1_ps(kernel.data[8]);

    constexpr int SSE_FLOATS = 4;

    for (int i = 0; i < out_height; i++) {
        int base_i = i * params.stride;

        int j = 0;
        for (; j <= out_width - SSE_FLOATS; j += SSE_FLOATS) {
            int base_j = j * params.stride; 

            __m128 sum = _mm_setzero_ps();

            // Row -1
            __m128 r0 = _mm_loadu_ps(&image.data[(base_i + 0) * image.width + base_j + 0]);
            __m128 r1 = _mm_loadu_ps(&image.data[(base_i + 0) * image.width + base_j + 1]);
            __m128 r2 = _mm_loadu_ps(&image.data[(base_i + 0) * image.width + base_j + 2]);

            sum = _mm_add_ps(sum, _mm_mul_ps(r0, k00));
            sum = _mm_add_ps(sum, _mm_mul_ps(r1, k01));
            sum = _mm_add_ps(sum, _mm_mul_ps(r2, k02));

            // Row 1
            r0 = _mm_loadu_ps(&image.data[(base_i + 1) * image.width + base_j + 0]);
            r1 = _mm_loadu_ps(&image.data[(base_i + 1) * image.width + base_j + 1]);
            r2 = _mm_loadu_ps(&image.data[(base_i + 1) * image.width + base_j + 2]);

            sum = _mm_add_ps(sum, _mm_mul_ps(r0, k10));
            sum = _mm_add_ps(sum, _mm_mul_ps(r1, k11));
            sum = _mm_add_ps(sum, _mm_mul_ps(r2, k12));

            // Row 2
            r0 = _mm_loadu_ps(&image.data[(base_i + 2) * image.width + base_j + 0]);
            r1 = _mm_loadu_ps(&image.data[(base_i + 2) * image.width + base_j + 1]);
            r2 = _mm_loadu_ps(&image.data[(base_i + 2) * image.width + base_j + 2]);

            sum = _mm_add_ps(sum, _mm_mul_ps(r0, k20));
            sum = _mm_add_ps(sum, _mm_mul_ps(r1, k21));
            sum = _mm_add_ps(sum, _mm_mul_ps(r2, k22));

            _mm_storeu_ps(&output.data[i * out_width + j], sum);
        }

        for (; j < out_width; j++) {
            int base_j = j * params.stride;
            float s = 0.f;
            for (int ky = 0; ky < 3; ky++)
                for (int kx = 0; kx < 3; kx++)
                    s += image.data[(base_i + ky) * image.width + (base_j + kx)] *
                         kernel.data[ky * 3 + kx];

            output.data[i * out_width + j] = s;
        }
    }

    return res;
}

static int conv2d_avx(
    const Conv2DParams& params, 
    Image& output
) {
    int res = CODE_SUCCESS;

    Image image = params.image;
    Kernel kernel = params.kernel;

    int out_height = (image.height - kernel.size) / params.stride + 1;
    int out_width  = (image.width - kernel.size) / params.stride + 1;
    int out_size   = out_height * out_width;

    output.height = out_height;
    output.width  = out_width;
    output.data   = new float[out_size];

    __m256 k00 = _mm256_set1_ps(kernel.data[0]);
    __m256 k01 = _mm256_set1_ps(kernel.data[1]);
    __m256 k02 = _mm256_set1_ps(kernel.data[2]);

    __m256 k10 = _mm256_set1_ps(kernel.data[3]);
    __m256 k11 = _mm256_set1_ps(kernel.data[4]);
    __m256 k12 = _mm256_set1_ps(kernel.data[5]);

    __m256 k20 = _mm256_set1_ps(kernel.data[6]);
    __m256 k21 = _mm256_set1_ps(kernel.data[7]);
    __m256 k22 = _mm256_set1_ps(kernel.data[8]);

    constexpr int AVX_FLOATS = 8;

    for (int i = 0; i < out_height; i++) {
        int base_i = i * params.stride;

        int j = 0;
        for (; j <= out_width - AVX_FLOATS; j += AVX_FLOATS) {
            int base_j = j * params.stride; 

            __m256 sum = _mm256_setzero_ps();

            // Row 0
            __m256 r0 = _mm256_loadu_ps(&image.data[(base_i + 0) * image.width + base_j + 0]);
            __m256 r1 = _mm256_loadu_ps(&image.data[(base_i + 0) * image.width + base_j + 1]);
            __m256 r2 = _mm256_loadu_ps(&image.data[(base_i + 0) * image.width + base_j + 2]);

            sum = _mm256_add_ps(sum, _mm256_mul_ps(r0, k00));
            sum = _mm256_add_ps(sum, _mm256_mul_ps(r1, k01));
            sum = _mm256_add_ps(sum, _mm256_mul_ps(r2, k02));

            // Row 1
            r0 = _mm256_loadu_ps(&image.data[(base_i + 1) * image.width + base_j + 0]);
            r1 = _mm256_loadu_ps(&image.data[(base_i + 1) * image.width + base_j + 1]);
            r2 = _mm256_loadu_ps(&image.data[(base_i + 1) * image.width + base_j + 2]);

            sum = _mm256_add_ps(sum, _mm256_mul_ps(r0, k10));
            sum = _mm256_add_ps(sum, _mm256_mul_ps(r1, k11));
            sum = _mm256_add_ps(sum, _mm256_mul_ps(r2, k12));

            // Row 2
            r0 = _mm256_loadu_ps(&image.data[(base_i + 2) * image.width + base_j + 0]);
            r1 = _mm256_loadu_ps(&image.data[(base_i + 2) * image.width + base_j + 1]);
            r2 = _mm256_loadu_ps(&image.data[(base_i + 2) * image.width + base_j + 2]);

            sum = _mm256_add_ps(sum, _mm256_mul_ps(r0, k20));
            sum = _mm256_add_ps(sum, _mm256_mul_ps(r1, k21));
            sum = _mm256_add_ps(sum, _mm256_mul_ps(r2, k22));

            _mm256_storeu_ps(&output.data[i * out_width + j], sum);
        }

        for (; j < out_width; j++) {
            int base_j = j * params.stride;
            float s = 0.f;
            for (int ky = 0; ky < 3; ky++)
                for (int kx = 0; kx < 3; kx++)
                    s += image.data[(base_i + ky) * image.width + (base_j + kx)] *
                         kernel.data[ky * 3 + kx];

            output.data[i * out_width + j] = s;
        }
    }

    return res;
}
