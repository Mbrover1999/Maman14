#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 100
#define MAX_LABEL_LEN 31

#define ATTR_CODE   1
#define ATTR_DATA   2
#define ATTR_EXTERN 4
#define ATTR_ENTRY  8

typedef struct {
    char name[MAX_LABEL_LEN];
    int value;
    int attributes;
} Symbol;

typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    int count;
} SymbolTable;

/* functions */

void init_symbol_table(SymbolTable *table);

int add_symbol(SymbolTable *table,
               const char *name,
               int value,
               int attributes);

Symbol *find_symbol(SymbolTable *table,
                    const char *name);

void print_symbol_table(SymbolTable *table);

#endif