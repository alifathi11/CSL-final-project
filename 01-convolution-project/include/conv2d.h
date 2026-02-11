#pragma once  

struct Image {
    float *data;
    int height;
    int width;
    int channels;
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

int conv2d_channels(
    int engine_mode, 
    const Conv2DParams& params, 
    Image& output
);