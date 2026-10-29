#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "preprocessor.h"
#include "macro_table.h"
#include "instruction_table.h"
#include "utils.h"
#include "errors.h"

int run_preprocessor(const char *input_filename, const char *output_filename, macro_table *m_table) {
    /* step 1: open input file (.as) for reading and output file (.am) for writing */
    FILE *in_file = fopen(input_filename, "r");
    FILE *out_file;
    char line[MAX_LINE_LENGTH + 2]; /* buffer for reading a line (+2 for \n and \0) */
    int line_number = 0;
    int in_macro_def = 0;          /* flag to check if we are currently inside a macro definition */
    macro *current_macro = NULL;   /* pointer to the current macro being updated */

    /* variable declarations at the top of the block to comply with ansi-c standard */
    char *cleaned_line;
    char line_copy[MAX_LINE_LENGTH + 2];
    char *token;
    char *macro_name;
    macro *m;

    /* step 1 (continued): check if the input file was opened successfully */
    if (!in_file) {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_filename);
        return 0;
    }

    /* step 1 (continued): open the output file for writing */
    out_file = fopen(output_filename, "w");
    if (!out_file) {
        fprintf(stderr, "Error: Cannot open output file %s\n", output_filename);
        fclose(in_file);
        return 0;
    }

    /* step 2: initialize the macro table (reset macro counter to 0) */
    init_macro_table(m_table);

    /* step 3: main loop - read the next line from the input file until end-of-file (eof) */
    while (fgets(line, sizeof(line), in_file) != NULL) {
        line_number++;
        cleaned_line = skip_whitespaces(line); /* skip leading spaces and tabs */

        /* handle empty lines and comment lines (save to macro if inside definition, or copy to output) */
        if (strlen(cleaned_line) == 0 || cleaned_line[0] == ';') {
            if (!in_macro_def) {
                fputs(line, out_file);
            } else {
                add_line_to_macro(line, current_macro);
            }
            continue;
        }

        /* create a copy of the line and break it into tokens to inspect the first word */
        strcpy(line_copy, cleaned_line);
        token = strtok(line_copy, " \t\n\r");

        if (token == NULL) continue;
        
        
        /* step 4: check if the first word is "mcro" (start of a macro definition) */
        if (strcmp(token, "mcro") == 0) {
            if (in_macro_def) {
                report_error(line_number, "nested macro definitions are not allowed");
            } 
            else {
                in_macro_def = 1;
                current_macro = NULL;
            
                /* get the next word which should be the new macro's name */
                macro_name = strtok(NULL, " \t\n\r");

                /* validation checks for the macro name (extra arguments, reserved words, or duplicates) */
                if (!macro_name) {
                    report_error(line_number, "missing macro name");
                } else if (strtok(NULL, " \t\n\r") != NULL) {
                    report_error(line_number, "extra characters after macro name");
                } else if (is_reserved_word(macro_name)) {
                    report_error(line_number, "macro name cannot be a reserved word");
                } else if (find_macro(m_table, macro_name)) {
                    report_error(line_number, "macro already defined");
                } else {
                    /* step 4 a: add the new macro to the table and update the current macro pointer */
                    int idx = add_macro(m_table, macro_name, line_number);
                    if (idx == -1) {
                        report_error(line_number, "exceeded maximum number of macros");
                        in_macro_def = 0;
                    } else {
                        current_macro = &m_table->macros[idx];
                        /* in_macro_def = 1;  turn flag on: we are now inside a macro definition */
                    }
                }
            }
        /* step 5: check if the first word is "mcroend" (end of a macro definition) */
        } else if (strcmp(token, "mcroend") == 0) {
            if (!in_macro_def) {
                report_error(line_number, "mcroend outside of macro definition");
            } else if (strtok(NULL, " \t\n\r") != NULL) {
                report_error(line_number, "extra characters after mcroend");
            } else {
                /* step 5 a: close the macro definition and reset the flag and pointer */
                in_macro_def = 0;
                current_macro = NULL;
            }
        } else if (strstr(cleaned_line, "mcro") != NULL){
            report_error(line_number, "there are characters before macro");
        } else {
            /* step 6: if the flag is on, add the current line to the macro's body in the table */
            if (in_macro_def) {
                if(current_macro != NULL){
                    add_line_to_macro(line, current_macro);
                }
            } else {
                /* step 7: if not in a definition, check if the first word is a call to an existing macro */
                m = find_macro(m_table, token);
                if (m) {
                    if (strtok(NULL, " \t\n\r") != NULL) {
                        report_error(line_number, "extra characters after macro call");
                    } else {
                        /* step 7 a: expand the macro by writing all its stored lines to the output file */
                        write_macro(out_file, m);
                    }
                } else {
                    /* step 8: if it's a regular line, copy it as-is to the output file */
                    fputs(line, out_file);
                }
            }
        }
    } /* go back to step 3 as long as there are more lines in the file */

    /* step 9: close both input and output files when done processing */
    fclose(in_file);
    fclose(out_file);

    /* safety check: verify we didn't reach eof with an unclosed macro definition */
    if (in_macro_def) {
        report_error(line_number, "end of file reached inside macro definition");
    }

    /* delete the output .am file if any errors occurred during preprocessing */
    if (has_errors()) {
        remove(output_filename);
        return 0;
    }

    return 1;
}
