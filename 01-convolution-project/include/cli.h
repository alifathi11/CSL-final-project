#pragma once 

#include <string> 

struct CLIArgs {

    int run_mode;

    int engine_mode;

    int kernel_type;
    int kernel_size;

    std::string input;
    std::string output;

    bool help = false;
};

void print_help();
int parse_cli(int argc, char **argv, CLIArgs& args);
int validate_cli(const CLIArgs& args);
