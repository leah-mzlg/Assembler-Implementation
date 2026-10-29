#ifndef MMN_14_DATA_IMAGE_H
#define MMN_14_DATA_IMAGE_H

/* adds a single numerical value to the data image */
void add_number_to_data_image(int value, int *dc);

/* adds a string of characters to the data image, including null terminator */
void add_string_to_data_image(const char *str, int *dc);

/* returns the value at a specific index in the data image */
int get_data_value(int index);

/* resets the data counter and the image for a new file */
void reset_data_image();

int get_current_data_size();

#endif
