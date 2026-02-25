#pragma once 

#include <string> 
#include "conv2d.h"

// std::string build_output_filename(
    
// ) {
//     // TODO: build output filename based on input params 
// }

int load_grayscale_image(
    const char *filename, 
    Image& image
);

int save_float_array_as_grayscale_image(
    const char *filename, 
    const Image& output
);

int load_rgb_image(
    const char* filename,
    Image& image
);

int save_float_array_as_rgb_image(
    const char* filename,
    const Image& image
);

int load_kernel_from_file(
    const char* path,
    Kernel& kernel
);

float* load_matrix(
    const char* path,
    int rows, 
    int cols
);

float* load_vector(
    const char* path, 
    int size
);

int resize_image(
    Image& image, 
    int new_width, 
    int new_height
);

int load_tensor(
    const std::string& path,
    Image& image
);