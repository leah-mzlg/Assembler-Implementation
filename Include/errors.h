#ifndef MMN_14_ERRORS_H
#define MMN_14_ERRORS_H

/* print error  and increments the error counter */
void report_error(int line_number, const char *message);

/* return 1 if were errors and 0 if all correct */
int has_errors();

/* resets the counter (when switching between different input files) */
void reset_errors();

#endif
