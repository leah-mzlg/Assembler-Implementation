#ifndef MMN_14_INSTRUCTION_TABLE_H
#define MMN_14_INSTRUCTION_TABLE_H

#define NO_OPERAND 0
#define INVALID_METHOD -1
#define METHOD_0 (1 << 0)
#define METHOD_1 (1 << 1)
#define METHOD_2 (1 << 2)
#define METHOD_3 (1 << 3)

/* structs */
typedef struct{
    char* name;
    int funct;
    int opcode;
    int operands_number;
    int source_methods;
    int destination_methods;
}instruction;

/* functions */
int is_reserved_word(const char *name);

int is_instruction_name(const char start[]);

const instruction* get_instruction(const char *name);


#endif
