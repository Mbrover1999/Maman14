#include <stdio.h>
#include <string.h>
#include "../headers/assembler_table.h"
/*Initiate the table*/
void init_assembler_table(AssemblerTable *table) {
    table->count = 0;
}
/*Find if symbol exists in the assembler table and return it, if not return NULL*/
Symbol *find_symbol(AssemblerTable *table, const char *name) {
    int i = 0;
    for (; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }

    return NULL;
}

/*Add a symbol to the table, if there are any problems return 0*/
int add_symbol(AssemblerTable *table, const char *name, int value, int attributes) {

    /*Check if table is at max symbols if so return 0*/
    if (table->count >= MAX_SYMBOLS) {
        return 0;
    }

    /*Check if symbol exist if so return 0*/
    if (find_symbol(table, name) != NULL) {
        return 0;
    }

    /* Checks are done, it is safe to add the symbol and it's attributes return 1*/
    strcpy(table->symbols[table->count].name, name);
    table->symbols[table->count].value = value;
    table->symbols[table->count].attributes = attributes;

    table->count++;

    return 1;
}


/*Print table for debugging*/
void print_assembler_table(AssemblerTable *table) {
    int i = 0;
    for (; i < table->count; i++) {
        printf("%s %d %d\n",
               table->symbols[i].name,
               table->symbols[i].value,
               table->symbols[i].attributes);
    }
}