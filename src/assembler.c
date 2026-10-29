#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "preprocessor.h"
#include "first_pass.h"
#include "second_pass.h"
#include "output_files.h"
#include "symbol_table.h"
#include "code_image.h"
#include "data_image.h"
#include "macro_table.h"
#include "errors.h"

int process_assembly_file(const char *base_filename) {
    char *as_filename;
    char *am_filename;
    macro_table m_table;
    FILE *am_file;
    int success_1;
        int success_2;

    /* allocate memory for filenames with extensions .as and .am */
    as_filename = malloc(strlen(base_filename) + 5);
    am_filename = malloc(strlen(base_filename) + 5);
    if (as_filename == NULL || am_filename == NULL) {
        fprintf(stderr, "error: memory allocation failed in assembler orchestrator\n");
        free(as_filename);
        free(am_filename);
        return 0;
    }

    sprintf(as_filename, "%s.as", base_filename);
    sprintf(am_filename, "%s.am", base_filename);

    /* initialize systems and reset modules before starting a new file */
    reset_errors();
    reset_code_image();
    reset_data_image();
    init_macro_table(&m_table);

    printf("--- starting processing for file: %s ---\n", as_filename);

    /* run the preprocessor to expand macros and generate the .am file */
    if (!run_preprocessor(as_filename, am_filename, &m_table)) {
        fprintf(stderr, "error: preprocessor failed for file %s\n", as_filename);
        free(as_filename);
        free(am_filename);
        return 0;
    }

    /* open the expanded .am file for reading during passes */
    am_file = fopen(am_filename, "r");
    if (am_file == NULL) {
        fprintf(stderr, "error: cannot open expanded file %s\n", am_filename);
        free(as_filename);
        free(am_filename);
        return 0;
    }

    /* run the first pass algorithm */
    success_1 = first_pass(am_file, &m_table);
    if (!success_1 || has_errors()) {
        fprintf(stderr, "error: first pass failed for file %s. output files will not be generated.\n", base_filename);
        /*fclose(am_file);
        free(as_filename);
        free(am_filename);
        free_symbol_table();
        return 0;*/
    }

    /* run the second pass algorithm */
    success_2 = second_pass(am_file);
    fclose(am_file); /* close the .am file as it is no longer needed */

    if (!success_2 || has_errors()) {
        fprintf(stderr, "error: second pass failed for file %s. output files will not be generated.\n", base_filename);
        free(as_filename);
        free(am_filename);
        free_symbol_table();
        return 0;
    }

    /* if no errors were found, generate the final output files (.ob, .ent, .ext) */
    printf("compilation completed successfully for %s. generating output files...\n", base_filename);
    success_2 = generate_output_files(base_filename, get_final_ic(), get_final_dc());
    if (!success_2) {
        fprintf(stderr, "error: failed to create output files for %s\n", base_filename);
    }

    /* clean up dynamic resources allocated for this file compile cycle */
    free(as_filename);
    free(am_filename);
    free_symbol_table();

    return success_2;
}
