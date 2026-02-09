#include "constants.h"
#include "functional_test.h"
#include "speed_test.h"
#include "utility.h"

// static void print_usage(const char *program_name) {
//     fprintf(stderr, "Usage:\n");
//     fprintf(stderr, "%s <input-image-path> <output-image-path>\n", program_name);
// }

static int run_interactive() {

    int res = CODE_SUCCESS;

    int option_def = CODE_SPEED_TEST;

    int option;

    OptionEntry options[2];
    options[0].option_number = CODE_FUNCTIONAL_TEST;
    options[0].option_name   = FUNCTIONAL_TEST_STR;
    options[1].option_number = CODE_SPEED_TEST;
    options[1].option_name   = SPEED_TEST_STR;

    res = read_option("Option", options, 2, stdin, &option_def, &option);
    if (res != CODE_SUCCESS) {
        print_err("Failed to read option", res);
        return res;
    }

    switch (option) {
        case CODE_FUNCTIONAL_TEST: 
            res = run_functional_test();
            break;

        case CODE_SPEED_TEST:
            res = run_speed_test();
            break;

        default:
            print_err("Invalid option", CODE_FAILURE_INVALID_INPUT);
            return CODE_FAILURE;
    }

    return res;
}

static int run_cli(int argc, char **argv) {
    // TODO: implement 

    // suppress warning
    if (argc == 1 || !argv) 
        return CODE_FAILURE_INVALID_ARG;

    return CODE_SUCCESS;
}

int main(int argc, char **argv) {

    int res = CODE_SUCCESS;

    if (argc == 1) res = run_interactive();
    else           res = run_cli(argc, argv);

    return res;
}