#include <iostream> 
#include <getopt.h>
#include <cstdlib>

#include "cli.h"
#include "constants.h"
#include "utility.h"

static struct option long_options[] = {
    {"mode",    required_argument, nullptr, 'm'},
    {"engine",  required_argument, nullptr, 'e'},
    {"ktype",   required_argument, nullptr, 'k'},
    {"ksize",   required_argument, nullptr, 's'},
    {"input",   required_argument, nullptr, 'i'},
    {"output",  required_argument, nullptr, 'o'},
    {"color",   required_argument, nullptr, 'c'},
    {"help",    no_argument,       nullptr, 'h'},
    {nullptr,   0,                 nullptr, 0}
};

void print_help() {
    std::cout <<
    "Usage: conv2d [OPTIONS]\n\n"
    "Modes:\n"
    "  -m --mode functional | speed\n\n"
    "Options:\n"
    "  -e, --engine   baseline | sse | avx\n"
    "  -k, --ktype    sharpen | blur | edge\n"
    "  -s, --ksize    kernel size (e.g. 3)\n"
    "  -i, --input    input file or directory\n"
    "  -o, --output   output file or directory (optional)\n"
    "  -c, --color    grayscale | rgb (optional) (default = rgb)\n"
    "  -h, --help     show this help\n";
}

static int get_run_mode_by_name(const std::string& run_mode_name) {
    if (run_mode_name == "functional") 
        return RUN_MODE_FUNCTIONAL_TEST;
    if (run_mode_name == "speed")
        return RUN_MODE_SPEED_TEST;

    return RUN_MODE_NONE;
}

static int get_engine_mode_by_name(const std::string& engine_mode_name) {
    if (engine_mode_name == "baseline")
        return ENGINE_MODE_BASELINE;
    if (engine_mode_name == "sse")
        return ENGINE_MODE_SSE;
    if (engine_mode_name == "avx")
        return ENGINE_MODE_AVX;

    return ENGINE_MODE_NONE;
}

static int get_kernel_type_by_name(const std::string& kernel_type_name) {
    if (kernel_type_name == "sharpen")
        return KERNEL_TYPE_SHARPEN;
    if (kernel_type_name == "blur")
        return KERNEL_TYPE_BLUR;
    if (kernel_type_name == "sobel_x")
        return KERNEL_TYPE_SOBEL_X;
    if (kernel_type_name == "sobel_y")
        return KERNEL_TYPE_SOBEL_Y;

    return KERNEL_TYPE_NONE;
}

static int get_color_mode_by_name(const std::string& color_mode_name) {
    if (color_mode_name == "grayscale")
        return COLOR_MODE_GRAYSCALE;
    if (color_mode_name == "rgb")
        return COLOR_MODE_RGB;

    return COLOR_MODE_NONE;
}

int parse_cli(int argc, char **argv, CLIArgs& args) {
    
    int opt; 
    int option_idx = 0;
    
    while ((opt = getopt_long(
        argc, argv,
        "m:e:k:s:i:o:c:h",
        long_options, 
        &option_idx
    )) != -1) {
        switch (opt) {
            case 'm':
                args.run_mode = get_run_mode_by_name(optarg);
                break;
            
            case 'e': 
                args.engine_mode = get_engine_mode_by_name(optarg);
                break;
            
            case 'k':
                args.kernel_type = get_kernel_type_by_name(optarg);
                break;
            
            case 's':
                args.kernel_size = std::atoi(optarg);
                break;

            case 'i':
                args.input = optarg; 
                break;
            
            case 'o':
                args.output = optarg; 
                args.save_output = !args.output.empty();
                break;

            case 'c':
                args.color_mode = get_color_mode_by_name(optarg);
                break;

            case 'h':
                args.help = true;
                break;
            
            default: 
                return CODE_FAILURE_INVALID_ARG;
        }
    }

    return CODE_SUCCESS;
}

int validate_cli(CLIArgs& args) {

    // TODO: reuse conv2d validation functions

    if (args.run_mode == RUN_MODE_NONE) {
        print_err("Invalid run mode", CODE_FAILURE_INVALID_ARG);
        return CODE_FAILURE_INVALID_ARG;
    }

    if (args.engine_mode == ENGINE_MODE_NONE) {
        print_err("Invalid engine mode", CODE_FAILURE_INVALID_ARG);
        return CODE_FAILURE_INVALID_ARG;  
    }

    if (args.kernel_type == KERNEL_TYPE_NONE) {
        print_err("Invalid kernel type", CODE_FAILURE_INVALID_ARG);
        return CODE_FAILURE_INVALID_ARG;
    }

    if (args.kernel_size <= 0) {
        print_err("Invalid kernel size", CODE_FAILURE_INVALID_ARG);
        return CODE_FAILURE_INVALID_ARG;  
    }

    if (args.input.empty()) {
        print_err("Input is required", CODE_FAILURE_ARG_REQUIRED);
        return CODE_FAILURE_ARG_REQUIRED; 
    }

    if (args.output.empty()) {
        print_warn("will not save the output images"); 
        args.save_output = false;
    }

    if (args.color_mode == COLOR_MODE_NONE) {
        print_warn("will set color mode to RGB");
        args.color_mode = COLOR_MODE_RGB;
    }
    
    return CODE_VALIDATION_OK;
}