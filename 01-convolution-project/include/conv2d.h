#pragma once  

struct Conv2DParams {
    int img_height;
    int img_width; 
    int kernel_size; 
    int stride;
};

bool is_valid_conv2d_params(const Conv2DParams& params);
bool is_valid_image_size(int img_height, int img_width);
bool is_valid_kernel_size(int kernel_size);

int conv2d_baseline(
    const float *image,
    const float *kernel,
    const Conv2DParams& params, 
    float *output
);

int conv2d_sse(
    const float *image,
    const float *kernel,
    const Conv2DParams& params, 
    float *output
);

int conv2d_avx(
    const float *image,
    const float *kernel,
    const Conv2DParams& params, 
    float *output
);