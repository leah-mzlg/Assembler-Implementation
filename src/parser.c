#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "utils.h"
#include "errors.h"
#include "macro_table.h"
#include "instruction_table.h"

char *parse_label(char *line, int line_number) {
    char *colon_ptr = strchr(line, ':');
    char *start = line;
    char *label;
    int len;

    /* there is no symbol at this line */
    if (colon_ptr == NULL) return NULL;

    /* skip whitespace at the start of the line */
    while (isspace((unsigned char) *start) && start < colon_ptr) {
        start++;
    }

    len = colon_ptr - start;
    if (len <= 0) {
        report_error(line_number, "colon found but no label name provided");
        return NULL;
    }

    label = malloc(len + 1); /* +1 for '\0'*/
    if (!label) return NULL;

    strncpy(label, start, len);
    label[len] = '\0';

    return label;
}

int is_valid_symbol_name(const char *name, macro_table *m_table, int line_number) {
    int i;
    int len = strlen(name);

    /* check length (assuming MAX_SYMBOL_NAME is 31 as per instructions) */
    if (len == 0 || len > MAX_SYMBOL_LENGTH) {
        report_error(line_number, "symbol name length must be between 1-31 characters");
        return 0;
    }

    /* check if starts with a letter */
    if (!isalpha(name[0])) {
        report_error(line_number, "symbol name must start with a letter");
        return 0;
    }

    /* check if all characters are alphanumeric */
    for (i = 1; i < len; i++) {
        if (!isalnum(name[i])) {
            report_error(line_number, "symbol name must contain only alphanumeric characters");
            return 0;
        }
    }

    /* check if it's a reserved word (instructions or Registers), this function is called from instruction_table.c */
    if (is_reserved_word(name)) {
        report_error(line_number, "symbol name cannot be a reserved assembly word");
        return 0; /* returns 1 if reserved, so here return 0 for "not valid as symbol" */
    }

    /* check if it's a macro name in the macro_table */
    if (m_table != NULL) {
        for (i = 0; i < m_table->macro_count; i++) {
            if (strcmp(name, m_table->macros[i].name) == 0) {
                report_error(line_number, "symbol name cannot be an existing macro name");
                return 0;
            }
        }
    }
    return 1;
}

directive_type identify_directive(char *start) {
    /* check if the directive name is valid and followed by space or end of string */
    if (strncmp(start, ".data", 5) == 0 && (isspace((unsigned char) start[5]) || start[5] == '\0'))
        return DATA_DIRECTIVE;
    if (strncmp(start, ".string", 7) == 0 && (isspace((unsigned char) start[7]) || start[7] == '\0'))
        return STRING_DIRECTIVE;
    if (strncmp(start, ".entry", 6) == 0 && (isspace((unsigned char) start[6]) || start[6] == '\0'))
        return ENTRY_DIRECTIVE;
    if (strncmp(start, ".extern", 7) == 0 && (isspace((unsigned char) start[7]) || start[7] == '\0'))
        return EXTERN_DIRECTIVE;

    return UNKNOWN_DIRECTIVE;
}

line_type determine_line_type(char *line, int line_number) {
    /* skips white spaces at the beginning */
    char *start = skip_whitespaces(line);
    char arr[5] = "";
    int i;
    /* check if the line is empty */
    if (*start == '\0' || *start == '\n' || *start == '\r') {
        return LINE_EMPTY;
    }

    /* check if it is a comment line */
    if (*start == ';') {
        return LINE_COMMENT;
    }
    
    /* check if it is a directive (starts with '.') */
    if (*start == '.') {
        if (identify_directive(start) != UNKNOWN_DIRECTIVE) {
            return LINE_DIRECTIVE;
        }
        /* dot found but not a known directive */
        report_error(line_number, "invalid directive name, Supported: .data, .string, .entry, .extern");
        return LINE_ERROR;
    }
    for(i = 0; i < 5 && start[i] != '\t' && start[i] != ' ' && start[i] != '\n' && start[i] != '\0'  ; i++){
      arr[i] = start[i];
    }
    arr[i] = '\0';
    /* check if it is an instruction (like mov, add...) */
    /* is_instruction_name will check if the word exists in the instruction table */
    if (is_instruction_name(arr)) {
        return LINE_INSTRUCTION;
    }

    /* no directive, no instruction, and no comment */
    report_error(line_number, "undefined command or invalid syntax");
    return LINE_ERROR;
}

directive_type get_directive_type(char *line) {
    return identify_directive(skip_whitespaces(line));
}

int *parse_data_parameters(char *parameters, int *count, int line_number) {

    int *data_array = NULL;
    char *parameters_copy;
    char *current_token;
    int found_numbers = 0;
    if (has_consecutive_commas(parameters)) {
    report_error(line_number, "Multiple consecutive commas detected");
    return NULL;
    }
    *count = 0;
    parameters = skip_whitespaces(parameters);

    if (*parameters == '\0') {
        report_error(line_number, "no parameters found after data directive");
        return NULL;
    }

    parameters_copy = my_strdup(parameters);
    if (!parameters_copy) return NULL;

    current_token = strtok(parameters_copy, ",");
    while (current_token != NULL) {
        current_token = trim_whitespaces(current_token);

        if (strlen(current_token) > 0) {
            /* strict validation: is it really a number? */
            if (!is_valid_numeric_string(current_token)) {
                report_error(line_number, "invalid character in data parameter");
                free(parameters_copy);
                free(data_array);
                return NULL;
            }

            found_numbers++;
            data_array = realloc(data_array, found_numbers * sizeof(int));
            if (data_array == NULL) {
                free(parameters_copy);
                return NULL;
            }

            data_array[found_numbers - 1] = atoi(current_token);
        }
        else {
            report_error(line_number, "illegal comma or empty parameter");
            free(parameters_copy);
            free(data_array);
            return NULL;
        }
        current_token = strtok(NULL, ",");
    }

    free(parameters_copy);
    *count = found_numbers;
    return data_array;
}

char* parse_string_parameter(char *parameter, int line_number) {
    char *start_quote, *end_quote;
    char *result_string;
    int string_length;

    /* skip white spaces to find the first quote */
    parameter = skip_whitespaces(parameter);

    /* check if it starts with opening quotes */
    if (*parameter != '\"') {
        report_error(line_number, "missing opening quotes for string");
        return NULL;
    }
    start_quote = parameter;

    /* find the last quote in the line */
    end_quote = strrchr(start_quote + 1, '\"');

    /* check if closing quotes exist and are not the same as start */
    if (end_quote == NULL || end_quote == start_quote) {
        report_error(line_number, "missing closing quotes for string");
        return NULL;
    }

    /* check if there is any "garbage" after the closing quotes */
    if (*skip_whitespaces(end_quote + 1) != '\0') {
        report_error(line_number, "extra characters after string end");
        return NULL;
    }

    /* calculate length (without the quotes) */
    string_length = end_quote - start_quote - 1;

    /* allocate memory for the clean string plus null terminator */
    result_string = (char *)malloc(string_length + 1);
    if (result_string == NULL) {
        return NULL;
    }

    /* copy the content between quotes to the new string */
    strncpy(result_string, start_quote + 1, string_length);
    result_string[string_length] = '\0';

    return result_string;
}

int get_addressing_method(char *operand, macro_table *m_table, int line_number) {
    if (operand == NULL || *operand == '\0') {
        return NO_OPERAND;
    }

    /* method 0: immediate (#number) */
    if (operand[0] == '#') {
        if (is_valid_numeric_string(operand + 1)) return METHOD_0;
        report_error(line_number, "invalid numeric value after '#'");
        return INVALID_METHOD;
    }

    /* method 3: register (r0-r7) */
    if (operand[0] == 'r' && strlen(operand) == 2 && operand[1] >= '0' && operand[1] <= '7') {
        return METHOD_3;
    }

    /* method 2: relative (%label) */
    if (operand[0] == '%') {
        if (is_valid_symbol_name(operand + 1, m_table, line_number)) return METHOD_2;
        return INVALID_METHOD; /* error already reported by is_valid_symbol_name */
    }

    /* method 1: direct (label) */
    if (is_valid_symbol_name(operand, m_table, line_number)) {
        return METHOD_1;
    }

    return INVALID_METHOD;
}

char** split_operands(char *line, int expected_count, int line_number) {

    char **operands;
    char *copy, *token;
    int count = 0;

    line = skip_whitespaces(line);
    if (*line == '\0') {
        if (expected_count > 0) report_error(line_number, "missing operands");
        return NULL;
    }
    
    if (has_consecutive_commas(line)) {
        report_error(line_number, "Multiple consecutive commas detected");
        return NULL;
    }

    copy = my_strdup(line);
    operands = malloc(sizeof(char*) * 2); /* max 2 operands allowed */

    token = strtok(copy, ",");
    while (token != NULL) {
        count++;
        if (count > expected_count) {
            report_error(line_number, "too many operands");
            free(copy);
            return NULL;
        }

        token = trim_whitespaces(token);
        if (strlen(token) == 0) {
            report_error(line_number, "illegal comma");
            free(copy);
            return NULL;
        }
        operands[count - 1] = my_strdup(token);
        token = strtok(NULL, ",");
    }

    if (count < expected_count) {
        report_error(line_number, "not enough operands");
        free(copy);
        return NULL;
    }

    free(copy);
    return operands;
}

int validate_addressing_methods(const instruction *inst, char **operands, macro_table *m_table, int line_number) {
    int i, method, allowed_methods;

    for (i = 0; i < inst -> operands_number; i++) {
        /* get the method of the current operand */
        method = get_addressing_method(operands[i], m_table, line_number);

        /* if identification failed, an error was already reported inside get_addressing_method */
        if (method == INVALID_METHOD) return 0;
        
        if (method == METHOD_2 && inst->opcode != 9) {
            report_error(line_number, "relative addressing (method 2) is only valid for jump instructions (jmp, bne, jsr)");
            return 0;
        }
        
        /* determine if we check source or destination rules */
        allowed_methods = (inst -> operands_number == 2 && i == 0) ? inst -> source_methods : inst -> destination_methods;

        /* check if the current method is allowed for this specific instruction */
        if (!(method & allowed_methods)) {
            report_error(line_number, "operand uses an illegal addressing method");
            return 0;
        }
    }
    return 1;
}

int count_instruction_words(const instruction *inst) {
    /* each instruction line takes 1 word for the word of instruction, each operand adds exactly 1 additional word */
    return 1 + inst -> operands_number;
}
