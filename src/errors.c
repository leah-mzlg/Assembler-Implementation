#include <stdio.h>
#include "errors.h"

static int error_counter = 0;

void report_error(int line_number, const char *message) {
    fprintf(stderr, "error in line %d: %s\n", line_number, message);
    error_counter++;
}

int has_errors() {
    return error_counter > 0;
}

void reset_errors() {
    error_counter = 0;
}
