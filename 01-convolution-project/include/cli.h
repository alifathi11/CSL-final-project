#pragma once 

#include <string> 

#include "constants.h"

struct CLIArgs {

    int run_mode = RUN_MODE_NONE;

    int engine_mode = ENGINE_MODE_NONE;

    int kernel_type = KERNEL_TYPE_NONE;

    int kernel_size = 0;

    std::string input;
    std::string output;
    
    std::string kernel_path;
    std::string fc_weight_path;
    std::string fc_bias_path;

    int color_mode = COLOR_MODE_NONE;

    bool eval = false;

    bool help = false;
    bool save_output = false;
};

void print_help();
int parse_cli(int argc, char **argv, CLIArgs& args);
int validate_cli(CLIArgs& args);
