#ifndef MMN_14_MACRO_TABLE_H
#define MMN_14_MACRO_TABLE_H

#include "utils.h"

/* defines */
#define MAX_NAME 31
#define MAX_LINES 100
#define MAX_MACROS 50


/* structs */
/* saves macro as a line in the macro table below */
typedef struct {
    char name[MAX_NAME];
    char lines[MAX_LINES][MAX_LINE_LENGTH]; /* save the macro's code */
    int line_count; /* save the amount of the lines of the macro's code */
}macro;

/* saves all the macros in the current input file */
typedef struct {
    macro macros[MAX_MACROS];
    int macro_count;
}macro_table;


/* functions */
void init_macro_table(macro_table* table); /* initialize the macros table */
int add_macro(macro_table* table, const char* name, int line_number); /* adds a new macro to the macro table, returns the new macro index in the table */
void add_line_to_macro(const char* line, macro* macro); /* adds a new line (=chars array) to macro in the macro table */
macro* find_macro(macro_table* table, const char* name); /* look for the current macro from the input file in the macro_table */
void write_macro(FILE* output, macro* m); /* write the macro code to the .am output file instead of the macro name */

#endif
