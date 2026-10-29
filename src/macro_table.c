#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro_table.h"
#include "errors.h"


void init_macro_table(macro_table* table) {
    table -> macro_count = 0;
}

int add_macro(macro_table* table, const char* name, int line_number) {
    if (table -> macro_count >= MAX_MACROS) {
        return -1;
    }
    strcpy(table -> macros[table -> macro_count].name, name);/* insert the new macro name in new line in the macro table */
    table -> macros[table -> macro_count].line_count = 0;/* initialize the line_count of the new macro to be 0 */
    table -> macro_count++;
    return table -> macro_count - 1;
}

void add_line_to_macro(const char* line, macro* macro) {
    if (macro -> line_count >= MAX_LINES) return; /* more than maximum of the lines in this macro */
    /* if (strlen(line) >= MAX_LINE_LENGTH) return;  too long line */

    strcpy(macro -> lines[macro -> line_count], line);
    macro -> line_count++;
}

macro* find_macro(macro_table* table, const char* name) {
    int i;
    for (i = 0; i < table -> macro_count; i++){
        if (strcmp(table -> macros[i].name, name) == 0) /* the names are equals - this macro is in the macro_table already */
            return &table -> macros[i];
    }
    return NULL; /* this macro is not in the macro_table */
}

void write_macro(FILE* output, macro* macro) {
  int i;
  for(i = 0; i < macro -> line_count; i++){
    fputs(macro -> lines[i], output);
  }
}

