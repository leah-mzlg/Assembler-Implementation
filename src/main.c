#include <stdio.h>
#include "main.h"
#include "assembler.h"

int main(int argc, char *argv[]) {
    int i;
    int files_processed_successfully = 0;

    /* check if at least one file argument was provided in the command line */
    if (argc < 2) {
        fprintf(stderr, "warning: no input files provided\n");
        fprintf(stderr, "usage: %s <filename1> <filename2> ... (without extensions)\n", argv[0]);
        return 1;
    }

    /* loop through each argument provided in argv */
    for (i = 1; i < argc; i++) {
        /* process the current file using the master assembler controller */
        if (process_assembly_file(argv[i])) {
            files_processed_successfully++;
        } else {
            fprintf(stderr, "warning: processing failed for file argument: %s\n", argv[i]);
        }
        printf("\n"); /* insert an empty line separator between files outputs */
    }

    /* print final execution summary to the standard output */
    printf("--- assembler execution finished ---\n");
    printf("successfully processed %d out of %d files.\n", files_processed_successfully, argc - 1);

    return 0;
}



