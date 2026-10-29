#include <stdio.h>
#include <string.h>
#include "data_image.h"
#include "errors.h"
#include "utils.h"

/* the static array representing the data memory image */
static int data_image[MAX_MEMORY_SIZE];
static int current_data_size = 0;

void add_number_to_data_image(int value, int *dc) {
    if (current_data_size >= MAX_MEMORY_SIZE) {
        /* no line number available here, so we report a general memory error */
        report_error(-1, "data image overflow: too many data definitions");
        return;
    }
    /* store the value and increment both the internal size and the global dc */
    data_image[current_data_size] = value;
    current_data_size++;
    (*dc)++;
}

void add_string_to_data_image(const char *str, int *dc) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        add_number_to_data_image((int)str[i], dc);/* dc increments for each char in str */
    }
    /* add the null terminator at the end of the string */
    add_number_to_data_image(0, dc);
}

int get_data_value(int index) {
    if (index >= 0 && index < current_data_size) {/*cheks that the index in the current indexes range*/
        return data_image[index];
    }
    return 0;
}

void reset_data_image() {
    /* reset the counter to the beginning of the array */
    current_data_size = 0;
    /* fill the array with zeros to ensure a clean slate for the next file */
    memset(data_image, 0, sizeof(data_image));
}

int get_current_data_size() {
    return current_data_size;
}