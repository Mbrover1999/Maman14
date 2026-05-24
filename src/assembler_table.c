#include <stdio.h>
#include <string.h>
#include "../headers/assembler_table.h"

void init_assembler_table(AssemblerTable *table) {
    table->count = 0;
}

Symbol *find_symbol(AssemblerTable *table, const char *name) {
    int i = 0;
    for (; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }

    return NULL;
}

int add_symbol(AssemblerTable *table, const char *name, int value, int attributes) {
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

void print_assembler_table(AssemblerTable *table) {
    int i = 0;
    for (; i < table->count; i++) {
        printf("%s %d %d\n",
               table->symbols[i].name,
               table->symbols[i].value,
               table->symbols[i].attributes);
    }
}