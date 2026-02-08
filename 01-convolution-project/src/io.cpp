#include <opencv2/opencv.hpp>

#include "io.h"
#include "utility.h"
#include "constants.h"

int load_grayscale_image(
    const char *filename, 
    float*& image, 
    int& height, 
    int& width
) {
    // TODO: validation 

    cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (img.empty()) {
        print_err("Failed to load image", CODE_FAILURE_READ_INPUT);
        return CODE_FAILURE;
    }

    cv::Mat img_f;
    img.convertTo(img_f, CV_32F, 1.0  / 255.0);

    height = img_f.rows;
    width  = img_f.cols;

    image = new float[height * width];

    for (int i = 0; i < height; i++) {
        const float *row_ptr = img_f.ptr<float>(i);
        std::memcpy(
            image + i * width, 
            row_ptr,
            width * sizeof(float)
        );
    }

    return CODE_SUCCESS;
}

int save_float_array_as_grayscale_image(
    const char *filename, 
    const float *data,
    int height, 
    int width
) {
    // TODO: validation 

    cv::Mat img_f(height, width, CV_32F, const_cast<float*>(data));

    cv::Mat img_clamped;
    cv::min(img_f, 1.0f, img_clamped);
    cv::max(img_clamped, 0.0f, img_clamped);

    cv::Mat img_u8;
    img_clamped.convertTo(img_u8, CV_8U, 255.0);

    if (!cv::imwrite(filename, img_u8)) {
        print_err("Failed to write output image", CODE_FAILURE_WRITE_OUTPUT);
        return CODE_FAILURE;
    }

    return CODE_SUCCESS;
}