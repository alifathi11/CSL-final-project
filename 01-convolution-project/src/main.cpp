#include <iostream>

#include "../include/conv2d.h"

int main() {
    const int imageWidth = 5;
    const int imageHeight = 5;
    const int kernelSize = 3;
    const int stride = 2;

    float image[imageHeight * imageWidth] = {
        1, 2, 3, 4, 5,
        6, 7, 8, 9,10,
       11,12,13,14,15,
       16,17,18,19,20,
       21,22,23,24,25
    };

    float kernel[kernelSize * kernelSize] = {
        1, 0,-1,
        1, 0,-1,
        1, 0,-1
    };

    const int outHeight = (imageHeight - kernelSize) / stride + 1;
    const int outWidth = (imageWidth - kernelSize) / stride + 1;
    const int outSize = outHeight * outWidth;
    float output[outSize]= {};

    Conv2DParams params;
    params.imageHeight = imageHeight;
    params.imageWidth = imageWidth;
    params.kernelSize = kernelSize;
    params.stride = stride;

    conv2d_baseline(image, kernel, params, output);

    for (int i = 0; i < outHeight; ++i) {
        for (int j = 0; j < outWidth; ++j) {
            int idx = i * outWidth + j;
            std::cout << output[idx] << " ";
        }
        std::cout << "\n";
    }

    return 0;
}