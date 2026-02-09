#pragma once  

struct Image {
    float *data;
    int height;
    int width;
};

struct Kernel {
    float *data;
    int type; 
    int size; 
};

struct Conv2DParams {
    Image image;
    Kernel kernel;
    int stride;
};

int conv2d(
    int engine_mode, 
    const Conv2DParams& params, 
    Image& output
);