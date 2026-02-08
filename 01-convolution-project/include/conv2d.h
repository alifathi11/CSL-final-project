#pragma once  

struct Conv2DParams {
    int img_height;
    int img_width; 
    int kernel_size; 
    int stride;
};

int conv2d_baseline(
    const float *image,
    const float *kernel,
    const Conv2DParams& params, 
    float *output
);

int conv2d_simd(
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