#ifndef MMN_14_PREPROCESSOR_H
#define MMN_14_PREPROCESSOR_H

#include <stdio.h>
#include "macro_table.h"

/* processes the .as file, outputs the .am file, and populates the given macro table
   returns 1 on success, 0 on failure (file not found, no valid macro name) */
int run_preprocessor(const char *input_filename, const char *output_filename, macro_table *m_table);

#endif
