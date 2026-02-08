#include <stdio.h>

#include "../include/utility.h"
#include "../include/constants.h"

void print_err(const char *msg, int errcode) {
    fprintf(stderr, "%s %s (CODE: %d)\n", LOG_LEVEL_ERROR, msg, errcode);
}