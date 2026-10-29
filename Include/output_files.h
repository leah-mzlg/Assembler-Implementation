#ifndef MMN_14_OUTPUT_FILES_H
#define MMN_14_OUTPUT_FILES_H

#include <stdio.h>

/* generates all the output files (.ob, .ent, .ext) based on the compiled memory images.
   base_filename is the name of the file without extension (e.g., "ps").
   returns 1 on success, 0 on failure.
*/
int generate_output_files(const char *base_filename, int final_ic, int final_dc);

#endif
