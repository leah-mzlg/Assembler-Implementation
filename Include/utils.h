#ifndef MMN_14_UTILS_H
#define MMN_14_UTILS_H

#define MAX_LINE_LENGTH 81
#define MAX_SYMBOL_LENGTH 32
#define MAX_MEMORY_SIZE 4096
#define MAX_INSTRUCTION_NAME_LENGTH 4

/* functions */
char* skip_whitespaces(char *str); /* skip whitespaces on start of strings */
char *trim_whitespaces(char *str); /*removes whitespaces from start and edn of strings*/
int is_valid_numeric_string(char *str); /* checks if a string represents a valid integer (can start with + or -) */
char *my_strdup(const char *str);
int has_consecutive_commas(const char *str);
#endif
