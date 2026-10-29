#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output_files.h"
#include "symbol_table.h"
#include "code_image.h"
#include "data_image.h"

/* helper function to write the .ob file */
static int write_object_file(const char *base_filename, int final_ic, int final_dc) {
    FILE *ob_file;
    char *filename;
    int addr;
    int i;
    code_word *word_ptr;

    /* allocate memory for the filename plus extension */
    filename = malloc(strlen(base_filename) + 5);
    if (filename == NULL) {
        return 0;
    }
    sprintf(filename, "%s.ob", base_filename);

    ob_file = fopen(filename, "w");
    if (ob_file == NULL) {
        free(filename);
        return 0;
    }

    /* write the header: total instruction words and total data words */
    fprintf(ob_file, "  %d %d\n", final_ic - 100, final_dc);

    /* write all encoded instructions from the code image */
    for (addr = 100; addr < final_ic; addr++) {
        word_ptr = get_code_word(addr);
        if (word_ptr != NULL) {
            /* print address as 4 digits and word as 3 hex digits with its are flag */
            /* adjust the formatting string here if your specific mamman requires a different base */
            fprintf(ob_file, "%04d %03X %c\n", addr, word_ptr->word, word_ptr->are);
        }
    }

    /* write all encoded data from the data image */
    for (i = 0; i < final_dc; i++) {
        /* data memory starts immediately after the final instruction counter */
        /* mask data to 12 bits (0xFFF) to ensure correct representation */
        fprintf(ob_file, "%04d %03X A\n", final_ic + i, get_data_value(i) & 0xFFF);
    }

    fclose(ob_file);
    free(filename);
    return 1;
}

/* helper function to handle entry symbols and write the .ent file if needed */
static int handle_entries(const char *base_filename) {
    FILE *ent_file;
    char *filename;

    /* check if there are any entry symbols using the provided function */
    if (!has_entry_symbols()) {
        return 1; /* no entry symbols found, nothing to do, not an error */
    }

    filename = malloc(strlen(base_filename) + 6);
    if (filename == NULL) {
        return 0;
    }
    sprintf(filename, "%s.ent", base_filename);

    ent_file = fopen(filename, "w");
    if (ent_file == NULL) {
        free(filename);
        return 0;
    }

    /* write all entry symbols using the provided function */
    write_entries_to_file(ent_file);

    fclose(ent_file);
    free(filename);
    return 1;
}

/* helper function to handle external references and write the .ext file if needed */
static int handle_externals(const char *base_filename, int final_ic) {
    FILE *ext_file;
    char *filename;

    /* check if there are any external references using the provided function */
    if (!has_external_references(final_ic)) {
        return 1; /* no external references found, nothing to do, not an error */
    }

    filename = malloc(strlen(base_filename) + 5);
    if (filename == NULL) {
        return 0;
    }
    sprintf(filename, "%s.ext", base_filename);

    ext_file = fopen(filename, "w");
    if (ext_file == NULL) {
        free(filename);
        return 0;
    }

    /* write all external references using the provided function */
    write_externals_to_file(ext_file, final_ic);

    fclose(ext_file);
    free(filename);
    return 1;
}

/* main orchestrator for creating output files */
int generate_output_files(const char *base_filename, int final_ic, int final_dc) {
    /* write the object file */
    if (!write_object_file(base_filename, final_ic, final_dc)) {
        return 0;
    }

    /* write the entries file if entry symbols exist */
    if (!handle_entries(base_filename)) {
        return 0;
    }

    /* write the externals file if external references exist */
    if (!handle_externals(base_filename, final_ic)) {
        return 0;
    }

    return 1;
}