#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <vector>

#include "conv2d.h"
#include "io.h"
#include "speed_test.h"
#include "constants.h"
#include "kernel_factory.h"
#include "utility.h"

int read_speed_test_params(SpeedTestParams& speed_test_params) {

    int res = CODE_SUCCESS;

    // Default values
    int engine_mode_def = ENGINE_MODE_BASELINE;
    int kernel_type_def = KERNEL_TYPE_SHARPEN;
    int kernel_size_def = KERNEL_SIZE_3;
    
    const std::string input_dir_def  = "./images/normal";
    const std::string output_dir_def = "";

    // Variables
    int engine_mode;
    int kernel_type; 
    int kernel_size;

    char input_dir [MED_BUF_SIZE];
    char output_dir[MED_BUF_SIZE];

    OptionEntry engine_modes[3];
    engine_modes[0].option_number = ENGINE_MODE_BASELINE;
    engine_modes[0].option_name   = ENGINE_MODE_BASELINE_STR;
    engine_modes[1].option_number = ENGINE_MODE_SSE;
    engine_modes[1].option_name   = ENGINE_MODE_SSE_STR;
    engine_modes[2].option_number = ENGINE_MODE_AVX;
    engine_modes[2].option_name   = ENGINE_MODE_AVX_STR;

    res = read_option("Engine Mode", engine_modes, 3, stdin, &engine_mode_def, &engine_mode);
    if (res != CODE_SUCCESS) {
        print_err("Failed to read engine mode", CODE_FAILURE_READ_INPUT);
        return res;
    }

    OptionEntry kernel_types[3];
    kernel_types[0].option_number = KERNEL_TYPE_SHARPEN;
    kernel_types[0].option_name   = KERNEL_TYPE_SHARPEN_STR;
    kernel_types[1].option_number = KERNEL_TYPE_BLUR;
    kernel_types[1].option_name   = KERNEL_TYPE_BLUR_STR;
    kernel_types[2].option_number = KERNEL_TYPE_EDGE;
    kernel_types[2].option_name   = KERNEL_TYPE_EDGE_STR;

    res = read_option("Kernel Type", kernel_types, 3, stdin, &kernel_type_def, &kernel_type);
    if (res != CODE_SUCCESS) {
        print_err("Failed to read kernel type", CODE_FAILURE_READ_INPUT);
        return res;
    }

    OptionEntry kernel_sizes[3];
    kernel_sizes[0].option_number = KERNEL_SIZE_3;
    kernel_sizes[0].option_name   = KERNEL_SIZE_3_STR;
    kernel_sizes[1].option_number = KERNEL_SIZE_5;
    kernel_sizes[1].option_name   = KERNEL_SIZE_5_STR;
    kernel_sizes[2].option_number = KERNEL_SIZE_7;
    kernel_sizes[2].option_name   = KERNEL_SIZE_7_STR;

    res = read_option("Kernel Size", kernel_sizes, 3, stdin, &kernel_size_def, &kernel_size);
    if (res != CODE_SUCCESS) {
        print_err("Failed to read kernel size", CODE_FAILURE_READ_INPUT);
        return res;
    }

    res = read_param("Input Directory", stdin, input_dir_def.c_str(), input_dir, sizeof(input_dir));
    if (res != CODE_SUCCESS) {
        print_err("Failed to read input directory", CODE_FAILURE_READ_INPUT);
        return res;
    }

    res = read_param("Output Directory", stdin, output_dir_def.c_str(), output_dir, sizeof(output_dir));
    if (res != CODE_SUCCESS) {
        print_err("Failed to read output directory", CODE_FAILURE_READ_INPUT);
        return res;
    }

    speed_test_params.engine_mode = engine_mode;
    speed_test_params.kernel_type = kernel_type;
    speed_test_params.kernel_size = kernel_size;
    speed_test_params.input_dir   = input_dir;
    speed_test_params.output_dir  = output_dir;
    speed_test_params.save_output = !speed_test_params.output_dir.empty();

    return res;
}

static int load_image_paths(
    const std::string& dir,
    std::vector<std::string>& image_paths
) {
    image_paths.clear();

    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        print_err("Failed to open the directory", CODE_FAILURE_FILE_NOT_FOUND);
        return CODE_FAILURE_FILE_NOT_FOUND; 
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file())
            continue;

        const auto& path = entry.path();
        std::string ext = path.extension().string();

        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") {
            image_paths.push_back(path.string());
        }
    }

    std::sort(image_paths.begin(), image_paths.end());

    return CODE_SUCCESS;
}


static int load_images(
    const std::string& dir, 
    std::vector<Image>& images
) {
    int res = CODE_SUCCESS;
    
    images.clear();
    
    std::vector<std::string> image_paths;

    res = load_image_paths(dir, image_paths);
    if (res != CODE_SUCCESS) {
        print_err("Failed to load image paths", res);
        return CODE_FAILURE;
    }

    for (auto& image_path : image_paths) {
        // TODO: currently we support just grayscale images
        Image image;
        
        res = load_grayscale_image(
                    image_path.c_str(), 
                    image);

        if (res != CODE_SUCCESS) {
            std::string err_msg = "Failed to load image: " + image_path;
            print_err(err_msg.c_str(), res);
            continue;
        }

        images.push_back(image);
    }

    return res;
}

static int save_images(
    const std::string& dir, 
    const std::vector<Image>& images
) {
    int res = CODE_SUCCESS;

    int output_number = 0;
    char output_filename[MED_BUF_SIZE];

    for (const auto& output : images) {

        snprintf(output_filename, sizeof(output_filename), "%s/out%d.jpeg", 
                dir.c_str(), output_number++);

        res = save_float_array_as_grayscale_image(
            output_filename, 
            output);

        if (res != CODE_SUCCESS) {
            std::string err_msg = "Failed to save image: " + std::string(output_filename);
            print_err(err_msg.c_str(), res);
            continue;
        }
    }

    return res;
}

int run_speed_test(const SpeedTestParams& speed_test_params) {

    int res = CODE_SUCCESS;

    std::vector<Image> input_images;
    std::vector<Image> output_images;

    Kernel kernel;

    int stride = 1;

    std::chrono::time_point<std::chrono::high_resolution_clock> t0, t1;
    std::chrono::duration<double, std::milli> elapsed;

    res = load_images(speed_test_params.input_dir, input_images);
    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    res = get_kernel(
        speed_test_params.kernel_type, 
        speed_test_params.kernel_size, 
        kernel);
    if (res != CODE_SUCCESS) {
        goto _exit;
    }

    elapsed = std::chrono::duration<double, std::milli>::zero();

    for (auto& image : input_images) {

        Conv2DParams conv2d_params = {
            image,
            kernel, 
            stride
        };

        t0 = std::chrono::high_resolution_clock::now();

        Image output;
        res = conv2d(
                speed_test_params.engine_mode,
                conv2d_params,
                output
        );

        t1 = std::chrono::high_resolution_clock::now();

        elapsed += t1 - t0;

        if (res != CODE_SUCCESS) {
            goto _exit;
        }

        output_images.push_back(output);
    }

    if (speed_test_params.save_output) {
        res = save_images(speed_test_params.output_dir, output_images);
        if (res != CODE_SUCCESS) {
            goto _exit;
        }
    }

    print_benchmark(speed_test_params.engine_mode, elapsed.count());

_exit: 
    for (auto& image : input_images) 
        delete[] image.data;

    for (auto& image : output_images)
        delete[] image.data;

    delete[] kernel.data;

    return res;
}