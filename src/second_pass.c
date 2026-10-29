#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "second_pass.h"
#include "symbol_table.h"
#include "encoder.h"
#include "code_image.h"
#include "errors.h"
#include "utils.h"
#include "parser.h"

int second_pass(FILE *input_file) {
    char line[MAX_LINE_LENGTH + 2];
    int line_number = 0;
    int ic = 100; /* instructions in memory always start at address 100 */

    /* ANSI C Variable Declarations - Must be at the very top of the function */
    char *cleaned_line;
    char *label;
    char *p;
    const instruction *inst;
    int words_count;
    int i;
    code_word *word;
    symbol_node *sym;
    symbol_type type;
    int sym_address;
    char arr[MAX_INSTRUCTION_NAME_LENGTH + 1] = "";/*save the instruction name*/
    /* rewind the file to the beginning for the second pass scan */
    rewind(input_file);

    /* step 1: Read the next line from the source file (.am) until EOF */
    while (fgets(line, sizeof(line), input_file) != NULL) {
        line_number++;

        /* skip empty lines and comment lines before processing algorithm steps */
        cleaned_line = skip_whitespaces(line);
        if (*cleaned_line == '\0' || *cleaned_line == '\n' || *cleaned_line == '\r' || *cleaned_line == ';') {
            continue;
        }

        /* step 2: check if the line starts with a label, if it does we skip it since labels were handled in pass 1 */
        label = parse_label(line, line_number);
        p = cleaned_line;
        if (label != NULL) {
            p = strchr(p, ':') + 1;
            p = skip_whitespaces(p);
            free(label); /* avoid memory leaks */
        }

        /* step 3: Check if the line is a directive statement (starts with '.') */
        if (*p == '.') {
            directive_type dir = identify_directive(p);

            /* step 4 -5: if it is an '.entry' directive, find the symbol in the table and mark its 'is_entry' flag */
            if (dir == ENTRY_DIRECTIVE) {
                p = skip_whitespaces(p + 6); /* Skip ".entry" */
                p = trim_whitespaces(p); /* Get the clean symbol name */

                if (!mark_as_entry(p)) {
                    report_error(line_number, "entry declared for a non-existent or external symbol");
                }
            }
            /* continue step 3: if it is any other directive (.data, .string, .extern), skip it and move to the next line */
            continue;
        }


        /* Step 7: The line is an instruction statement. Process operands and complete missing code words */

        for (i = 0; i < 5 && p[i] != '\t' && p[i] != ' ' && p[i] != '\n' && p[i] != '\0'; i++) {
            arr[i] = p[i];
        }
        arr[i] = '\0';
        if (is_instruction_name(arr)) {
            arr[i] = '\0';
            inst = get_instruction(arr);

            if (inst != NULL) {
                words_count = count_instruction_words(inst);

                /* Move past the first word (the operation word itself, which is already encoded) */
                ic++;

                /* Loop through the remaining extra words (operands) that might depend on labels */
                for (i = 1; i < words_count; i++) {
                    word = get_code_word(ic);

                    /* Inspect the code image to see if this specific word is waiting for a label address completion */
                    if (word != NULL && word->is_label_dependency) {
                        sym = find_symbol(word->label_name);

                        /* If the symbol does not exist in the symbol table, it's an "Undefined Symbol" error */
                        if (sym == NULL) {
                            report_error(word -> line_number, "undefined symbol used as an operand");
                        } else {
                            type = get_symbol_type(sym);
                            sym_address = get_symbol_address(sym);

                            /* if METHOD_2 */
                            if (word->is_relative) {
                                int base_instruction_address = ic - i;
                                int relative_distance = sym_address - base_instruction_address;

                                /* METHOD_2 is always 'A' */
                                complete_symbol_word(ic, relative_distance, 'A');
                            }


                            /* If the symbol is an external symbol, set its ARE to 'E' and write reference to the .ext file */
                            else if (type == EXTERNAL_SYMBOL) {
                                /* External symbol: ARE is 'E', address inside the word is 0 */
                                complete_symbol_word(ic, 0, 'E');
                            } else {
                                /* If the symbol is local (code/data), set its ARE to 'R' and encode its real address */
                                /* If the symbol is local (code/data), set its ARE to 'R' and encode its real address */
                                complete_symbol_word(ic, sym_address, 'R');
                            }
                        }
                    }
                    ic++; /* Update IC to point to the next memory address slot */
                }
            }
        }
    }

    /* Step 8: Check if any errors occurred during the entire pass. Return 1 (success) if no errors, 0 otherwise */
    return !has_errors();
}
