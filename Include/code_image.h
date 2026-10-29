#ifndef MMN_14_CODE_IMAGE_H
#define MMN_14_CODE_IMAGE_H

#include "utils.h"

/* represents a single word in the machine code image */
typedef struct {
    unsigned int word : 12; /* the machine word, exactly 12 bits */
    int is_label_dependency; /* 1 if address needs completion in second pass */
    int is_relative;
    char label_name[MAX_SYMBOL_LENGTH]; /* name of the label for the second pass */
    char are; /* A, R, or E */
    int line_number; /* line number in the input file */
} code_word;

/* adds a word to the code image */
void add_word_to_code_image(unsigned int word, int *ic, int line_number);

/* adds a placeholder word for a label that will be filled in the second pass */
void add_label_dependency_to_code_image(const char *label_name, int *ic, int line_number, int is_relative);

/* returns a pointer to a word at a specific address */
code_word* get_code_word(int address);

/* resets the code image for a new file */
void reset_code_image();

int get_current_code_size();

int has_external_references(int final_ic);

void write_externals_to_file(FILE *ext_file, int final_ic);


#endif
