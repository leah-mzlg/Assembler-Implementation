#include <stdio.h>
#include <string.h>

#include "code_image.h"
#include "utils.h"
#include "errors.h"


static code_word code_image[MAX_MEMORY_SIZE];/* the code image array */
static int current_code_size = 0;

void add_word_to_code_image(unsigned int word, int *ic, int line_number) {
    if (current_code_size >= MAX_MEMORY_SIZE) {
        report_error(-1, "code image overflow: too many instructions");
        return;
    }
    code_image[current_code_size].word = word;
    code_image[current_code_size].is_label_dependency = 0;
    code_image[current_code_size].label_name[0] = '\0';
    code_image[current_code_size].are = 'A';
    code_image[current_code_size].line_number = line_number;
    current_code_size++;
    (*ic)++;
}

void add_label_dependency_to_code_image(const char *label_name, int *ic, int line_number, int is_relative) {
    if (current_code_size >= MAX_MEMORY_SIZE) {
        report_error(-1, "code image overflow: too many instructions");
        return;
    }
    code_image[current_code_size].word = 0; /* will be filled in second pass */
    code_image[current_code_size].is_label_dependency = 1;
    code_image[current_code_size].is_relative = is_relative;
    strncpy(code_image[current_code_size].label_name, label_name, MAX_SYMBOL_LENGTH - 1);
    code_image[current_code_size].label_name[MAX_SYMBOL_LENGTH - 1] = '\0';
    code_image[current_code_size].are = '?';
    code_image[current_code_size].line_number = line_number;
    current_code_size++;
    (*ic)++;
}

code_word* get_code_word(int address) {
    /* instructions start from address 100 */
    int index = address - 100;/* indexes starts from 0 */
    if (index >= 0 && index < current_code_size) {
        return &code_image[index];
    }
    return NULL;
}

void reset_code_image() {
    /* reset the counter to the beginning of the array */
    current_code_size = 0;
    /* fill the array with zeros to ensure a clean slate for the next file */
    memset(code_image, 0, sizeof(code_image));
}

int get_current_code_size() {
    return current_code_size;
}

int has_external_references(int final_ic) {
    int addr;
    for (addr = 100; addr < final_ic; addr++) {
        code_word *word = get_code_word(addr);
        if (word != NULL && word->are == 'E') {
            return 1;
        }
    }
    return 0;
}

void write_externals_to_file(FILE *ext_file, int final_ic) {
    int addr;
    for (addr = 100; addr < final_ic; addr++) {
        code_word *word = get_code_word(addr);
        if (word != NULL && word->are == 'E') {
            fprintf(ext_file, "%s %04d\n", word->label_name, addr);
        }
    }
}

