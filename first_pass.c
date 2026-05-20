#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler_table.h"

void first_pass(FILE *in, AssemblerTable *table) {

    char line[256];
    int inside_macro = 0;
    Symbol *current_symbol = NULL;

    while (fgets(line, sizeof(line), in) != NULL) {

        char *ptr = line;


        // ignore spaces and move to the next str in line
        while (isspace((unsigned char) *ptr)) {
            ptr++;
        }

        // check if empty or comment, if so ignore and move on
        if (*ptr == '\0' || (*ptr == ';')) {
            continue;
        }


        char first_word[80];
        char second_word[80];

        first_word[0] = '\0';
        second_word[0] = '\0';

    }

}

