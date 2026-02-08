#pragma once 

struct Conv2DParams {
    int imageHeight;
    int imageWidth; 
    int kernelSize; 
    int stride;
};

int conv2d_baseline(
    const float *image,
    const float *kernel,
    const Conv2DParams& params, 
    float *output
);