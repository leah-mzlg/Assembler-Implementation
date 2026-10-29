#ifndef UNTITLED_SECOND_PASS_H
#define UNTITLED_SECOND_PASS_H
/* Runs the second pass on the expanded assembly file (.am).
   Receives the input file and the opened .ext file to write references directly.
   Returns 1 if the pass completed with no errors, or 0 if errors were found. */
int second_pass(FILE *input_file);
#endif
