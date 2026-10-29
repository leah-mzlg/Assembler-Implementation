#ifndef MMN_14_ASSEMBLER_H
#define MMN_14_ASSEMBLER_H

/* processes a single assembly source file.
   receives the base filename without extension (e.g., "input").
   returns 1 if the file was processed successfully with no errors, or 0 otherwise. */
int process_assembly_file(const char *base_filename);

#endif 
