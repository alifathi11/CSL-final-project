#pragma once 

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