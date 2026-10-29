#ifndef MMN_14_SYMBOL_TABLE_H
#define MMN_14_SYMBOL_TABLE_H

typedef enum {
    CODE_SYMBOL,    /* symbol pointing to a code instruction */
    DATA_SYMBOL,    /* symbol pointing to a data instruction */
    EXTERNAL_SYMBOL, /* symbol defined as external */
    INVALID_SYMBOL
} symbol_type;

typedef struct symbol_node symbol_node;

/* functions */
symbol_node* find_symbol(char *name); /* searches for a symbol by name */
int add_symbol(char *name, int address, symbol_type type); /* creates and adds a new node to the end of the list */
int mark_as_entry(char *name); /* updates the entry flag of an existing symbol */
void update_data_symbols_address(int icf); /* adds icf to all data symbols at the end of first pass */
symbol_type get_symbol_type(symbol_node *node);
int get_symbol_address(symbol_node *node);
int has_entry_symbols(void);
void write_entries_to_file(FILE *out_file);
void free_symbol_table(); /* releases the symbol table */

#endif


