#ifndef MMN_14_FIRST_PASS_H
#define MMN_14_FIRST_PASS_H

#include <stdio.h>
#include "macro_table.h"

/* runs the entire first pass algorithm line by line, returns 1 on success, 0 if errors were found */
int first_pass(FILE *input_file, macro_table *m_table);

/* returns the final value of the instruction counter (ic) */
int get_final_ic(void);

/* returns the final value of the data counter (dc) */
int get_final_dc(void);

#endif
