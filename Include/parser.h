#ifndef MMN_14_PARSER_H
#define MMN_14_PARSER_H

#include "macro_table.h"
#include "instruction_table.h"

typedef enum {
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_INSTRUCTION,
    LINE_DIRECTIVE,
    LINE_ERROR
} line_type;

typedef enum {
    DATA_DIRECTIVE,
    STRING_DIRECTIVE,
    ENTRY_DIRECTIVE,
    EXTERN_DIRECTIVE,
    UNKNOWN_DIRECTIVE
} directive_type;

/* functions */

/* returns the name of the label at the beginning of a line, if there is no label returns NULL
  'line' is a string representing the full line
  'line_number' is the no of the current line in the input file*/
char* parse_label(char *line, int line_number);

/* returns 1 if label name is valid and 0 otherwise */
int is_valid_symbol_name(const char *name, macro_table *m_table, int line_number);

/* helper function to identify the directive type from a string */
directive_type identify_directive(char *start);

/* returns an enum representing the type of line
  'line' is the line to process (after removing any labels) */
line_type determine_line_type(char *line, int line_number);

/* returns the corresponding directive_type enum value
  'line' is the string starting with the directive (e.g., ".data 1,2") */
directive_type get_directive_type(char* line);

/* parses parameters of a .data directive into an array of integers
  returns an allocated array of integers, or NULL on error
  'parameters' is the parameters string (e.g., "10, -5, 3")
  'count' is pointer to an integer that will store the number of parsed elements */
int* parse_data_parameters(char *parameters, int *count, int line_number);

/* parses the parameter of a .string directive and cleans quotation marks
  returns a cleaned string (e.g., "abc"), or NULL if the syntax is invalid
  'parameter' is the parameter string (e.g., "\"abc\"") */
char* parse_string_parameter(char *parameter, int line_number);

/* identifies the addressing method and returns its bitmask or INVALID_METHOD */
int get_addressing_method(char *operand, macro_table *m_table, int line_number);

/* splits a string into operands by comma, validates count and syntax */
char** split_operands(char *line, int expected_count, int line_number);

/* checks if the addressing methods of the operands match the instruction rules
  'inst' is the instruction of the current line
  '*operands' is the operands string array */
int validate_addressing_methods(const instruction *inst, char **operands, macro_table *m_table, int line_number);

/* main orchestrator for instruction line analysis */
int process_instruction_line(char *line, macro_table *m_table, int line_number);

/* returns the number of words (L) that the command will occupy in the machine's memory
   'inst' is the instruction of the current line */
int count_instruction_words(const instruction *inst);

/*
 stay to define in parser.c:
int process_instruction_line(char *line, macro_table *m_table, int line_number);
*/



#endif

