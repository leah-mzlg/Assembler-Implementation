#include <stdio.h>
#include "encoder.h"
#include "instruction_table.h"
#include "code_image.h"


int get_method_value(int method_bitmask) {
    if (method_bitmask == METHOD_0 || method_bitmask == NO_OPERAND) return 0;
    if (method_bitmask == METHOD_1) return 1;
    if (method_bitmask == METHOD_2) return 2;
    if (method_bitmask == METHOD_3) return 3;
    return 0;
}

unsigned int encode_instruction_word(const instruction *instruction_pointer, int source_method, int destination_method) {

    unsigned int instruction_word = 0;
    int source_method_value = get_method_value(source_method);
    int destination_method_value = get_method_value(destination_method);

    instruction_word = instruction_word | (instruction_pointer->opcode << 8); /* shift the opcode to bits 8-11 */
    instruction_word = instruction_word | (instruction_pointer -> funct << 4); /* shift the funct to bits 4-7 */
    instruction_word = instruction_word | (source_method_value << 2); /* shift the source method to bits 2-3 */
    instruction_word = instruction_word | (destination_method_value); /* shift the destination method to bits 2-3 */

    return instruction_word;
}

unsigned int encode_immediate_word(int immediate_value) {
    unsigned int immediate_word = (immediate_value & 0xFFF);
    return immediate_word;
}

unsigned int encode_register_word(int register_number) {
    unsigned int register_word;
    if (register_number < 0 || register_number > 7)/* register is not in valid range */
        return 0;

    register_word = (unsigned int)(1 << register_number); /* shift 1 to the register_number's bit */
    return register_word;
}

/* !! f o r   A d i  - s e c o n d   p a s s !!  */
void complete_symbol_word(int address, unsigned int calculated_word, char are_character) {
    /* 1. קבלת מצביע ישיר למילה במערך */
    code_word *word_ptr = get_code_word(address);

    /* הגנה מפני חריגה מהערכים (מצביע NULL) */
    if (word_ptr == NULL) {
        return;
    }

    /* 2. עדכון הכתובת עצמה - אנו מוודאים שלוקחים רק 12 סיביות */
    word_ptr->word = calculated_word & 0xFFF; 

    /* 3. עדכון תו ה-ARE (למשל 'R' או 'E') */
    word_ptr->are = are_character;

    /* 4. כיבוי הדגל - המילה הזו כבר לא זקוקה להשלמה */
    word_ptr->is_label_dependency = 0;
}
