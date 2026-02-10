#include "cli.h"
#include "constants.h"
#include "functional_test.h"
#include "speed_test.h"
#include "utility.h"

static int read_input_and_run_functional_test() {

    int res = CODE_SUCCESS;

    FunctionalTestParams params;
    
    res = read_functional_test_input(params);
    if (res != CODE_SUCCESS)
        return res;

    res = run_functional_test(params);
    
    return res;
}

static int read_input_and_run_speed_test() {
    
    int res = CODE_SUCCESS;

    SpeedTestParams params;

    res = read_speed_test_params(params);
    if (res != CODE_SUCCESS) 
        return res;

    res = run_speed_test(params);
    
    return res;
}

static int run_interactive() {

    int res = CODE_SUCCESS;

    int option_def = RUN_MODE_SPEED_TEST;

    int option;

    OptionEntry options[2];
    options[0].option_number = RUN_MODE_FUNCTIONAL_TEST;
    options[0].option_name   = RUN_MODE_FUNCTIONAL_TEST_STR;
    options[1].option_number = RUN_MODE_SPEED_TEST;
    options[1].option_name   = RUN_MODE_SPEED_TEST_STR;

    res = read_option("Option", options, 2, stdin, &option_def, &option);
    if (res != CODE_SUCCESS) {
        print_err("Failed to read option", res);
        return res;
    }

    if (option == RUN_MODE_FUNCTIONAL_TEST) 
        res = read_input_and_run_functional_test();
    else if (option == RUN_MODE_SPEED_TEST)
        res = read_input_and_run_speed_test();

    return res;
}

static int run_cli(int argc, char **argv) {

    int res = CODE_SUCCESS;

    CLIArgs args;

    res = parse_cli(argc, argv, args);
    if (res != CODE_SUCCESS || args.help) {
        print_help();
        return res;
    }

    res = validate_cli(args);
    if (res != CODE_VALIDATION_OK) {
        return res;
    }

    if (args.run_mode == RUN_MODE_FUNCTIONAL_TEST) {

        FunctionalTestParams params = {
            args.engine_mode, 
            args.kernel_type, 
            args.kernel_size, 
            args.input, 
            args.output,
            args.save_output
        };

        res = run_functional_test(params);
    }
    else if (args.run_mode == RUN_MODE_SPEED_TEST) {

        SpeedTestParams params = {
            args.engine_mode, 
            args.kernel_type,
            args.kernel_size,
            args.input, 
            args.output,
            args.save_output
        };

        res = run_speed_test(params);

    }
       
    return res;
}

int main(int argc, char **argv) {

    int res = CODE_SUCCESS;

    if (argc == 1) res = run_interactive();
    else           res = run_cli(argc, argv);

    return res;
}