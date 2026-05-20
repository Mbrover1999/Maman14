#include <stdio.h>
#include <string.h>
#include "assembler_table.h"

void init_symbol_table(SymbolTable *table) {
    table->count = 0;
}

Symbol *find_symbol(SymbolTable *table, const char *name) {
    int i;

    for (i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }

    return NULL;
}

int add_symbol(SymbolTable *table, const char *name, int value, int attributes) {
    if (table->count >= MAX_SYMBOLS) {
        return 0;
    }

    if (find_symbol(table, name) != NULL) {
        return 0;
    }

    strcpy(table->symbols[table->count].name, name);
    table->symbols[table->count].value = value;
    table->symbols[table->count].attributes = attributes;

    table->count++;

    return 1;
}

void print_symbol_table(SymbolTable *table) {
    int i;

    for (i = 0; i < table->count; i++) {
        printf("%s %d %d\n",
               table->symbols[i].name,
               table->symbols[i].value,
               table->symbols[i].attributes);
    }
}