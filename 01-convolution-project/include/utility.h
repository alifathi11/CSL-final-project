#pragma once 

void print_err(const char *msg, int errcode);

int load_grayscale_image(
    const char *filename, 
    float*& image, 
    int& height, 
    int& width
);

int save_float_array_as_grayscale_image(
    const char *filename, 
    const float *data,
    int height, 
    int width
);