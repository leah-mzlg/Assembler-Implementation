#ifndef MMN_14_ENCODER_H
#define MMN_14_ENCODER_H

#include "instruction_table.h"

/* gets the integer representation (0-3) from the METHOD_X bitmask */
int get_method_value(int method_bitmask);

/* encodes the first word of an instruction
   returns a 12-bit unsigned integer containing:
   bits 11-8: opcode
   bits 7-4: funct
   bits 3-2: source method
   bits 1-0: destination method */
unsigned int encode_instruction_word(const instruction *instruction_pointer, int source_method, int destination_method);

/* encodes an immediate numeric operand (method 0) into its 12-bit word
   the number is placed inside the word according to the specification
*/
unsigned int encode_immediate_word(int immediate_value);

/* encodes a register operand (method 3) into its 12-bit word
   receives the register number as an integer and shifts it to the correct bit position
*/
unsigned int encode_register_word(int register_number);

   /* updates a specific word in the code image during the second pass
   this completes a word that depends on a symbol address or relative distance */
void complete_symbol_word(int address, unsigned int calculated_word, char are_character);

#endif
