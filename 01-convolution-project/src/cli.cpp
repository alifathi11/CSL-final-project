#include <iostream> 
#include <getopt.h>
#include <cstdlib>

#include "cli.h"
#include "constants.h"
#include "utility.h"

static struct option long_options[] = {
    {"mode",      required_argument, nullptr, 'm'},
    {"engine",    required_argument, nullptr, 'e'},
    {"ktype",     required_argument, nullptr, 'k'},
    {"ksize",     required_argument, nullptr, 's'},
    {"kpath",     required_argument, nullptr, 'p'},
    {"fc_weight", required_argument, nullptr, 'w'},
    {"fc_bias",   required_argument, nullptr, 'b'},
    {"input",     required_argument, nullptr, 'i'},
    {"output",    required_argument, nullptr, 'o'},
    {"color",     required_argument, nullptr, 'c'},
    {"eval",     no_argument,       nullptr, 'v'},
    {"help",     no_argument,       nullptr, 'h'},
    {nullptr,    0,                 nullptr, 0}
};

void print_help() {
    std::cout <<
    "Usage: conv2d [OPTIONS]\n\n"
    "Modes:\n"
    "  -m --mode functional | speed | infer\n\n"
    "Options:\n"
    "  -e, --engine     baseline | sse | avx\n"
    "  -k, --ktype      kernel type (functional/speed only)\n"
    "  -s, --ksize      kernel size (functional/speed only)\n"
    "  -p, --kpath      path to conv kernel file (infer mode)\n"
    "  -w, --fc_weight  path to fully connected weight file (infer mode)\n"
    "  -b, --fc_bias    path to fully connected bias file (infer mode)\n"
    "  -i, --input      input file or directory\n"
    "  -o, --output     output file (optional)\n"
    "  -c, --color      grayscale | rgb (default = rgb)\n"
    "  -v, --eval       evaluate model\n"
    "  -h, --help       show this help\n";
}

static int get_run_mode_by_name(const std::string& run_mode_name) {
    if (run_mode_name == "functional") 
        return RUN_MODE_FUNCTIONAL_TEST;
    if (run_mode_name == "speed")
        return RUN_MODE_SPEED_TEST;
    if (run_mode_name == "infer")
        return RUN_MODE_INFER_TEST;

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
    if (kernel_type_name == "box_blur")
        return KERNEL_TYPE_BOX_BLUR;
    if (kernel_type_name == "gaussian_blur")
        return KERNEL_TYPE_GAUSSIAN_BLUR;
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
        "m:e:k:s:p:w:b:i:o:c:v:h",
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

            case 'p':
                args.kernel_path = optarg;
                break;

            case 'w':
                args.fc_weight_path = optarg;
                break;

            case 'b':
                args.fc_bias_path = optarg;
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

            case 'v':
                args.eval = true;
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

    if (args.run_mode == RUN_MODE_NONE) {
        print_err("Invalid run mode", CODE_FAILURE_INVALID_ARG);
        return CODE_FAILURE_INVALID_ARG;
    }

    if (args.engine_mode == ENGINE_MODE_NONE) {
        print_err("Invalid engine mode", CODE_FAILURE_INVALID_ARG);
        return CODE_FAILURE_INVALID_ARG;  
    }

    if (args.run_mode != RUN_MODE_INFER_TEST && args.kernel_type == KERNEL_TYPE_NONE) {
        print_err("Invalid kernel type", CODE_FAILURE_INVALID_ARG);
        return CODE_FAILURE_INVALID_ARG;
    }

    if (args.run_mode != RUN_MODE_INFER_TEST && args.kernel_size <= 0) {
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
    
    if (args.run_mode == RUN_MODE_INFER_TEST) {

        args.color_mode = COLOR_MODE_GRAYSCALE; // TODO: allow setting color mode for infer test
        print_warn("will set color mode to Grayscale for infer test");

        if (args.kernel_path.empty()) {
            print_err("Kernel path required in infer mode", CODE_FAILURE_ARG_REQUIRED);
            return CODE_FAILURE_ARG_REQUIRED;
        }

        if (args.fc_weight_path.empty()) {
            print_err("FC weight path required in infer mode", CODE_FAILURE_ARG_REQUIRED);
            return CODE_FAILURE_ARG_REQUIRED;
        }

        if (args.fc_bias_path.empty()) {
            print_err("FC bias path required in infer mode", CODE_FAILURE_ARG_REQUIRED);
            return CODE_FAILURE_ARG_REQUIRED;
        }
    }

    return CODE_VALIDATION_OK;
}