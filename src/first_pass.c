#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "first_pass.h"
#include "parser.h"
#include "symbol_table.h"
#include "instruction_table.h"
#include "data_image.h"
#include "code_image.h"
#include "errors.h"
#include "utils.h"
#include "encoder.h"

/* static counters to track the layout of machine memory image components */
static int ic; /* instruction counter starting at address 100 */
static int dc; /* data counter tracking total allocated data units */

int get_final_ic(void) {
    return ic;
}

int get_final_dc(void) {
    return dc;
}

int first_pass(FILE *input_file, macro_table *m_table) {
    /* safe buffer size containing room for standard line plus overflow detection elements */
    char line[MAX_LINE_LENGTH + 2];
    int line_number = 1;

    /* step 1: initialize the counters */
    ic = 100;
    dc = 0;

    /* step 2: read line by line until end of file */
    while (fgets(line, sizeof(line), input_file) != NULL) {
        char *line_pointer;
        char *label_name = NULL;
        int symbol_defined = 0;
        int line_len = strlen(line);
        line_type current_line_type;

        /* validate line length constraints to ensure no buffer slicing occurred */
        if (line_len > MAX_LINE_LENGTH - 1 && line[line_len - 1] != '\n' && !feof(input_file)) {
            report_error(line_number, "line is too long, maximum length is 80 characters");

            /* consume and skip the remaining overflow characters of this line from stream */
            while (fgets(line, sizeof(line), input_file) != NULL) {
                int temp_len = strlen(line);
                if (line[temp_len - 1] == '\n') break;
            }
            line_number++;
            continue;
        }

        /* check if empty line or comment line using parser functions */
        line_pointer = skip_whitespaces(line);
        if (*line_pointer == '\0' || *line_pointer == '\n' || *line_pointer == '\r' || *line_pointer == ';') {
            line_number++;
            continue;
        }
        
        /* step 3: check if label exists at the start of the line */
        label_name = parse_label(line_pointer, line_number);
        if (label_name != NULL) {
            /* step 4: validate the symbol name format */
            if (is_valid_symbol_name(label_name, m_table, line_number)) {
                symbol_defined = 1; /* safe flag active only for the lifecycle of the current line loop */
            }

            /* advance line reference past the label delimiter marker */
            line_pointer = strchr(line_pointer, ':');
            if (line_pointer != NULL) {
                line_pointer++;
            }
        }
        line_pointer = skip_whitespaces(line_pointer);
        /* step 5: determine the line type and check if directive */
        current_line_type = determine_line_type(line_pointer, line_number);

        if (current_line_type == LINE_EMPTY || current_line_type == LINE_COMMENT || current_line_type == LINE_ERROR) {
            line_number++;
            continue;
        }

        if (current_line_type == LINE_DIRECTIVE) {
            directive_type current_directive = identify_directive(line_pointer);

            /* check if the directive is .data or .string. if not, go to step 8 */
            if (current_directive == DATA_DIRECTIVE || current_directive == STRING_DIRECTIVE) {
                char *parameters = line_pointer;

                /* step 6: if a label was defined insert it into the symbol table with attributes and value = dc */
                if (symbol_defined) {
                    if (add_symbol(label_name, dc, DATA_SYMBOL) == 0) {
                        report_error(line_number, "symbol multiplication error: label already exists");
                    }
                }

                /* skip the directive keyword safely by tracking to the first argument whitespace gap */
                while (*parameters != '\0' && !isspace((unsigned char)*parameters)) {
                    parameters++;
                }

                /* step 7: parse values, encode them into the data image, and increase dc by the size. then return to step 2 */
                if (current_directive == DATA_DIRECTIVE) {
                    int count = 0; /* count numbers actually found */
                    int *parsed_numbers = parse_data_parameters(parameters, &count, line_number);

                    if (parsed_numbers != NULL) { /* if the numbers were parsed successfully with no syntax errors */
                        int i;
                        for (i = 0; i < count; i++) {
                            add_number_to_data_image(parsed_numbers[i], &dc); /* save the number into memory and automatically bump up the dc counter */
                        }
                        free(parsed_numbers);
                    }
                }
                else { /* current_directive == STRING_DIRECTIVE) */
                    char *parsed_string = parse_string_parameter(parameters, line_number);
                    if (parsed_string != NULL) {
                        add_string_to_data_image(parsed_string, &dc);
                        free(parsed_string);
                    }
                }
            }
            /* step 8: check if it is an .extern or .entry directive. if not, go to step 11 */
            else if (current_directive == EXTERN_DIRECTIVE) {
                char *extern_argument = line_pointer;
                char *extern_name;

                while (*extern_argument != '\0' && !isspace((unsigned char)*extern_argument)) {
                    extern_argument++; /* skip the word ".extern" itself by moving until hit a space */
                }
                extern_argument = skip_whitespaces(extern_argument);
                extern_name = strtok(extern_argument, " \t\n\r");

                if (extern_name == NULL) {
                    report_error(line_number, "missing symbol name after .extern directive");
                }
                else {
                    /* step 10: validate external symbol and add to table with address 0 */
                    if (is_valid_symbol_name(extern_name, m_table, line_number)) {
                        if (add_symbol(extern_name, 0, EXTERNAL_SYMBOL) == 0) {
                            symbol_node *existing = find_symbol(extern_name);
                            if (get_symbol_type(existing) != EXTERNAL_SYMBOL) {
                                report_error(line_number, "external symbol conflict with a local label definition");
                            }
                        }
                    }
                }
            }
            /* step 9 : check if entry directive */
            else if (current_directive == ENTRY_DIRECTIVE) {
                /* do nothing here, entry declarations are skipped and processed in second pass */
            }
            else {
                report_error(line_number, "unknown invalid directive name provided");
            }
        }
        /* step 11: if it is an instruction line */
        else if (current_line_type == LINE_INSTRUCTION) {
            char instruction_name[MAX_INSTRUCTION_NAME_LENGTH + 1];
            const instruction *inst;
            int i = 0;

            /* insert the label into the symbol table with value = ic and code attribute */
            if (symbol_defined) {
                if (add_symbol(label_name, ic, CODE_SYMBOL) == 0) {
                    report_error(line_number, "symbol multiplication error: label already exists");
                }
                /* else (add_symbol(label_name, ic, CODE_SYMBOL) == 1, the symbol adding to symbol_table succeed */
            }
            /* step 12: */
            /* extract dynamic instruction keyword prefix */
            while (line_pointer[i] != '\0' && !isspace((unsigned char)line_pointer[i]) && i < MAX_INSTRUCTION_NAME_LENGTH) {
                instruction_name[i] = line_pointer[i];
                i++;
            }
            instruction_name[i] = '\0';

            /* look up the instruction in the static table constraints */
            inst = get_instruction(instruction_name);
            if (inst == NULL) { /* this instruction name did not found */
                report_error(line_number, "invalid code opcode or instruction name detected");
            }
            else {
                char **operands_array;
                char *operands_section = skip_whitespaces(line_pointer + strlen(instruction_name));/* skip the instruction name at the line */

                /* step 13: parse operands and validate counts */
                operands_array = split_operands(operands_section, inst -> operands_number, line_number);
                if (operands_array != NULL || inst -> operands_number == 0) {

                    /* step 15: validate addressing methods matching */
                    if (inst -> operands_number == 0 || validate_addressing_methods(inst, operands_array, m_table, line_number)) {
                        int source_method = NO_OPERAND;
                        int destination_method = NO_OPERAND;
                        unsigned int base_word;

                        if (inst->operands_number == 2) {
                            source_method = get_addressing_method(operands_array[0], m_table, line_number);
                            destination_method = get_addressing_method(operands_array[1], m_table, line_number);
                        } else if (inst->operands_number == 1) {
                            destination_method = get_addressing_method(operands_array[0], m_table, line_number);
                        }

                        /* step 16: encode first word and add to code memory image array */
                        base_word = encode_instruction_word(inst, source_method, destination_method);
                        add_word_to_code_image(base_word, &ic, line_number);

                        /* check for operand configuration and register optimization paths */
                        if (inst->operands_number > 0) {
                            int idx;
                            /* push extension words independently based on addressing structures */
                            for (idx = 0; idx < inst->operands_number; idx++) {
                                int current_method = (inst->operands_number == 2 && idx == 0) ? source_method : destination_method;
                                char *current_op = operands_array[idx];

                                if (current_method == METHOD_0) {
                                    int val = atoi(current_op + 1); /* skip # and take the number only */
                                    unsigned int immediate_word = encode_immediate_word(val);

                                    add_word_to_code_image(immediate_word, &ic, line_number);
                                }
                                else if (current_method == METHOD_1) {
                                    add_label_dependency_to_code_image(current_op, &ic, line_number, 0);
                                }
                                else if (current_method == METHOD_2) {
                                    /* skip % and take the label name */
                                    add_label_dependency_to_code_image(current_op + 1, &ic, line_number, 1);
                                }
                                else if (current_method == METHOD_3) {
                                    int reg_num = current_op[1] - '0'; /* take the number of register from its name */
                                    unsigned int register_word = encode_register_word(reg_num);

                                    add_word_to_code_image(register_word, &ic, line_number);
                                }
                            }

                        }
                    }

                    /* free memory of the extracted operand strings array tokens */
                    if (operands_array != NULL) {
                        int k;
                        for (k = 0; k < inst->operands_number; k++) {
                            free(operands_array[k]);
                        }
                        free(operands_array);
                    }
                }
            }
        }

        /* step 17: free memory allocated for current label and go to next line */
        if (label_name != NULL) {
            free(label_name);
        }
        line_number++;
    }

    /* step 18: check if total memory size was exceeded */
    if ((ic + dc) >= MAX_MEMORY_SIZE) {
        report_error(-1, "assembler execution stopped: RAM memory capacity bounds exceeded");
    }

    /* step 19: check if any compilation errors occurred so far using official module state */
    if (has_errors()) {
        return 0; /* execution state explicitly marked as failure */
    }

    /* step 20: final update of all data symbols addresses using final ic counter value */
    update_data_symbols_address(ic);

    return 1; /* first pass finished successfully */
}
