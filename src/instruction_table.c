#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "instruction_table.h"

#define INSTRUCTION_COUNT 16

static instruction instruction_table[] = {
    {"mov", 0, 0, 2, METHOD_0 | METHOD_1 | METHOD_3, METHOD_1 | METHOD_3},
    {"cmp", 0, 1, 2, METHOD_0 | METHOD_1 | METHOD_3, METHOD_0 | METHOD_1 | METHOD_3},
    {"add", 10, 2, 2, METHOD_0 | METHOD_1 | METHOD_3, METHOD_1 | METHOD_3},
    {"sub", 11, 2, 2, METHOD_0 | METHOD_1 | METHOD_3, METHOD_1 | METHOD_3},
    {"lea", 0, 4, 2, METHOD_1, METHOD_1 | METHOD_3},
    {"clr", 10, 5, 1, NO_OPERAND, METHOD_1 | METHOD_3},
    {"not", 11, 5, 1, NO_OPERAND, METHOD_1 | METHOD_3},
    {"inc", 12, 5, 1, NO_OPERAND, METHOD_1 | METHOD_3},
    {"dec", 13, 5, 1, NO_OPERAND, METHOD_1 | METHOD_3},
    {"jmp", 10, 9, 1, NO_OPERAND, METHOD_1 | METHOD_2},
    {"bne", 11, 9, 1, NO_OPERAND, METHOD_1 | METHOD_2},
    {"jsr", 12, 9, 1, NO_OPERAND, METHOD_1 | METHOD_2},
    {"red", 0, 12, 1, NO_OPERAND, METHOD_1 | METHOD_3},
    {"prn", 0, 13, 1, NO_OPERAND, METHOD_0 | METHOD_1 | METHOD_3},
    {"rts", 0, 14, 0, NO_OPERAND, NO_OPERAND},
    {"stop", 0, 15, 0, NO_OPERAND, NO_OPERAND}
};

int is_reserved_word(const char *name) {
    int i;
    /* checking registers */
    if (name[0] == 'r' && strlen(name) == 2 && name[1] >= '0' && name[1] <= '7') {
        return 1;
    }
    /* checking instructions */
    for (i = 0; i < INSTRUCTION_COUNT; i++) {
        if (strcmp(name, instruction_table[i].name) == 0) {
            return 1;
        }
    }
    /* chacking directives */
    if (strcmp(name, "data") == 0 || strcmp(name, "string") == 0 || strcmp(name, "entry") == 0 || strcmp(name, "extern") == 0) {
        return 1;
    }
    return 0;
}

int is_instruction_name(const char start[]) {
    int i;


    for (i = 0; i < INSTRUCTION_COUNT; i++) {


        /* check if the string starts with the instruction name */
        if (strcmp(start, instruction_table[i].name) == 0) {
            return 1;
            /* ensure the word ends exactly here
               the next character must be a space, a tab, or the end of the string */

        }
    }
    return 0; /* no match found */
}

const instruction* get_instruction(const char *name) {
    int i;
    for (i = 0; i < INSTRUCTION_COUNT; i++) {
        if (strcmp(name, instruction_table[i].name) == 0) {
            return &instruction_table[i];
        }
    }
    return NULL; /* this 'name' is not an instruction */
}
