#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "utils.h"


struct symbol_node {
    char name[MAX_SYMBOL_LENGTH]; /* the label name*/
    int address; /* the memory address (ic for code, dc for data, 0 for external) */
    symbol_type type; /* code or data symbols are defined in this file and external symbols are defined in another file */
    int is_entry; /* entry flag (1/0) indicating if this symbol should be accessible to other files, only code or data symbols can be entries */
    struct symbol_node* next; /* pointer to the next node */
};

typedef struct {
    symbol_node *head;  /* pointer to the list start */
    symbol_node *tail;  /* pointer to the list end */
    int size; /* number of symbols */
} symbol_table;

static symbol_table symbols = {NULL, NULL, 0};


symbol_node* find_symbol(char *name) {
    symbol_node *current = symbols.head;
    while (current != NULL) {
        if (strcmp(current -> name, name) == 0) {
            return current; /* found the symbol */
        }
        current = current -> next;
    }
    return NULL; /* symbol not found */
}

int add_symbol(char *name, int address, symbol_type type) {
    symbol_node *new_node;

    /* first, check if symbol already exists to prevent redefinition */
    if (find_symbol(name) != NULL) {
        return 0; /* symbol already defined */
    }

    /* allocate memory for the new node */
    new_node = (symbol_node *)malloc(sizeof(symbol_node));
    if (new_node == NULL) {
        fprintf(stderr, "error: memory allocation failed for symbol %s\n", name);
        exit(1);
    }

    /* initialize node fields */
    strncpy(new_node -> name, name, 31);
    new_node -> name[31] = '\0'; /* ensure null-termination */
    new_node -> address = address;
    new_node -> type = type;
    new_node -> is_entry = 0; /* default: not an entry */
    new_node -> next = NULL;

    /* connect to the list using the tail for O(1) insertion */
    if (symbols.head == NULL) {
        symbols.head = new_node;
        symbols.tail = new_node;
    }
    else {
        symbols.tail-> next = new_node;
        symbols.tail = new_node;
    }

    symbols.size++;
    return 1; /* the new_node added to the table */
}

int mark_as_entry(char *name) {
    symbol_node *node = find_symbol(name);

    if (node == NULL || node -> type == EXTERNAL_SYMBOL) {
        return 0; /* symbol not found or external*/
    }
    node -> is_entry = 1;
    return 1; /* success - the symbol exists and is code or data */}

void update_data_symbols_address(int icf) {
    symbol_node *current = symbols.head;
    while (current != NULL) {
        /* if current symbol is of type symbol_data */
        if (current -> type == DATA_SYMBOL) {
            current -> address += icf; /* add the icf to the dc */
        }
        current = current -> next;
    }
}

int has_entry_symbols(void) {
    symbol_node *current_symbol = symbols.head;
    while (current_symbol != NULL) {
        if (current_symbol -> is_entry) return 1;/* one entry symbol found */
        current_symbol = current_symbol -> next;
    }
    return  0;
}

void write_entries_to_file(FILE *entries_file) {
    symbol_node *current_symbol = symbols.head;
    while (current_symbol != NULL) {
        if (current_symbol -> is_entry) {
            /* prints this symbol to the entries output file*/
            fprintf(entries_file, "%s %04d\n", current_symbol->name, current_symbol->address);
        }
        current_symbol = current_symbol -> next;
    }
}

symbol_type get_symbol_type(symbol_node *node) {
    if (node == NULL) return INVALID_SYMBOL;
    return (node -> type);
}

int get_symbol_address(symbol_node *node) {
    if (node == NULL) return 0;
    return (node -> address);
}

void free_symbol_table() {
    symbol_node *current = symbols.head;
    symbol_node *temp;

    while (current != NULL) {
        temp = current;
        current = current -> next;
        free(temp); /* free each node */
    }

    /* reset the table manager */
    symbols.head = NULL;
    symbols.tail = NULL;
    symbols.size = 0;
}

