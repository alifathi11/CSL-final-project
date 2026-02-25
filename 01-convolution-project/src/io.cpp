#include <fstream>
#include <opencv2/opencv.hpp>

#include "io.h"
#include "conv2d.h"
#include "utility.h"
#include "constants.h"

int load_grayscale_image(
    const char *filename, 
    Image& image
) {
    // TODO: validation 

    cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (img.empty()) {
        print_err("Failed to load image", CODE_FAILURE_READ_INPUT);
        return CODE_FAILURE;
    }

    cv::Mat img_f;
    img.convertTo(img_f, CV_32F, 1.0  / 255.0);

    image.height   = img_f.rows;
    image.width    = img_f.cols;
    image.channels = CHANNELS_GRAYSCALE;

    image.data = new float[image.height * image.width];

    for (int i = 0; i < image.height; i++) {
        const float *row_ptr = img_f.ptr<float>(i);
        std::memcpy(
            image.data + i * image.width, 
            row_ptr,
            image.width * sizeof(float)
        );
    }

    return CODE_SUCCESS;
}

int save_float_array_as_grayscale_image(
    const char *filename, 
    const Image& output
) {
    // TODO: validation 

    cv::Mat img_f(output.height, output.width, CV_32F, const_cast<float*>(output.data));

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

int load_rgb_image(
    const char *filename, 
    Image &image
) {
    cv::Mat img = cv::imread(filename, cv::IMREAD_COLOR);
    if (img.empty()) {
        print_err("Failed to load RGB image", CODE_FAILURE_READ_INPUT);
        return CODE_FAILURE;
    }

    cv::Mat img_f; 
    img.convertTo(img_f, CV_32F, 1.0 / 255.0);

    image.height   = img_f.rows;
    image.width    = img_f.cols;
    image.channels = CHANNELS_RGB;

    int plane = image.height * image.width;
    image.data = new float[plane * image.channels];

    std::vector<cv::Mat> bgr;
    cv::split(img_f, bgr);

    std::memcpy(image.data + 0 * plane, bgr[2].ptr<float>(), plane * sizeof(float));
    std::memcpy(image.data + 1 * plane, bgr[1].ptr<float>(), plane * sizeof(float));
    std::memcpy(image.data + 2 * plane, bgr[0].ptr<float>(), plane * sizeof(float));

    return CODE_SUCCESS;
}

int save_float_array_as_rgb_image(
    const char* filename,
    const Image& image
) {
    if (image.channels != CHANNELS_RGB) {
        print_err("Image is not in RGB format", CODE_FAILURE_INVALID_ARG);
        return CODE_FAILURE;
    }

    int h = image.height;
    int w = image.width;
    int plane = h * w;

    cv::Mat r(h, w, CV_32F, const_cast<float*>(image.data + 0 * plane));
    cv::Mat g(h, w, CV_32F, const_cast<float*>(image.data + 1 * plane));
    cv::Mat b(h, w, CV_32F, const_cast<float*>(image.data + 2 * plane));

    cv::min(r, 1.0f, r); cv::max(r, 0.0f, r);
    cv::min(g, 1.0f, g), cv::max(g, 0.0f, g);
    cv::min(b, 1.0f, b); cv::max(b, 0.0f, b);

    std::vector<cv::Mat> bgr = { b, g, r };

    cv::Mat img_f, img_u8;
    cv::merge(bgr, img_f);
    img_f.convertTo(img_u8, CV_8U, 255.0);

    if (!cv::imwrite(filename, img_u8)) {
        print_err("Failed to write RGB image", CODE_FAILURE_WRITE_OUTPUT);
        return CODE_FAILURE;
    }

    return CODE_SUCCESS;
}

int load_kernel_from_file(const char* path, Kernel &kernel) {

    std::ifstream in(path);
    if (!in.is_open()) {
        print_err("Failed to open kernel file", CODE_FAILURE_READ_INPUT);
        return CODE_FAILURE;
    }

    kernel.size = KERNEL_SIZE_3;
    kernel.data = new float[kernel.size * kernel.size];

    for (int i = 0; i < kernel.size * kernel.size; i++) {
        if (!(in >> kernel.data[i])) {
            print_err("Invalid kernel format", CODE_FAILURE_INVALID_INPUT);
            delete[] kernel.data;
            return CODE_FAILURE;
        }
    }

    in.close();
    return CODE_SUCCESS;
}

float* load_matrix(
    const char* path,
    int rows, 
    int cols
) {
    std::ifstream in(path);
    if (!in.is_open()) {
        print_err("Failed to open matrix file", CODE_FAILURE_READ_INPUT);
        return nullptr;
    }
    float* data = new float[rows * cols];
    for (int i = 0; i < rows * cols; i++) {
        if (!(in >> data[i])) {
            print_err("Invalid matrix format", CODE_FAILURE_INVALID_INPUT);
            delete[] data;
            return nullptr;
        }
    }

    in.close();
    return data;
}

float* load_vector(
    const char* path, 
    int size
) {
    std::ifstream in(path);
    if (!in.is_open()) {
        print_err("Failed to open vector file", CODE_FAILURE_INVALID_INPUT);
        return nullptr;
    }

    float* data = new float[size];

    for (int i = 0; i < size; i++) {
        if (!(in >> data[i])) {
            print_err("Invalid vector format", CODE_FAILURE_INVALID_INPUT);
            delete[] data;
            return nullptr;
        }
    }

    in.close();
    return data;
}

int resize_image(
    Image& image,
    int new_width,
    int new_height
) {
    int old_width  = image.width;
    int old_height = image.height;

    float* old_data = image.data;
    float* new_data = new float[new_width * new_height];

    float scale_x = static_cast<float>(old_width)  / new_width;
    float scale_y = static_cast<float>(old_height) / new_height;

    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {

            // PIL coordinate transform
            float src_x = (x + 0.5f) * scale_x - 0.5f;
            float src_y = (y + 0.5f) * scale_y - 0.5f;

            int x0 = static_cast<int>(floor(src_x));
            int y0 = static_cast<int>(floor(src_y));
            int x1 = x0 + 1;
            int y1 = y0 + 1;

            // Clamp
            x0 = std::max(0, std::min(x0, old_width - 1));
            x1 = std::max(0, std::min(x1, old_width - 1));
            y0 = std::max(0, std::min(y0, old_height - 1));
            y1 = std::max(0, std::min(y1, old_height - 1));

            float dx = src_x - x0;
            float dy = src_y - y0;

            float v00 = old_data[y0 * old_width + x0];
            float v01 = old_data[y0 * old_width + x1];
            float v10 = old_data[y1 * old_width + x0];
            float v11 = old_data[y1 * old_width + x1];

            // Bilinear interpolation
            float value =
                (1 - dx) * (1 - dy) * v00 +
                dx       * (1 - dy) * v01 +
                (1 - dx) * dy       * v10 +
                dx       * dy       * v11;

            new_data[y * new_width + x] = value;
        }
    }

    delete[] image.data;

    image.width  = new_width;
    image.height = new_height;
    image.data   = new_data;

    return CODE_SUCCESS;
}

int load_tensor(const std::string& path, Image& image)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
        return CODE_FAILURE;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size != 64 * 64 * sizeof(float)) {
        std::cerr << "Unexpected file size: " << size << std::endl;
        return CODE_FAILURE;
    }

    image.width = 64;
    image.height = 64;
    image.channels = 1;   // 🔥 YOU FORGOT THIS
    image.data = new float[64 * 64];

    if (!file.read(reinterpret_cast<char*>(image.data), size)) {
        std::cerr << "Read failed\n";
        return CODE_FAILURE;
    }

    return CODE_SUCCESS;
}