#include <stdlib.h>
#include <string.h>
#include "macro_helper.h"
const char *reserved_key_words[] = {
    /* commands */
    "mov", "cmp", "add", "sub",
    "lea", "clr", "not", "inc",
    "dec", "jmp", "bne", "red",
    "prn", "jsr", "rts", "stop",

    /* registers */
    "r0", "r1", "r2", "r3",
    "r4", "r5", "r6", "r7",

    /* directives */
    ".data",
    ".string",
    ".entry",
    ".extern",

};
    

const int reserved_key_words_length =
    sizeof(reserved_key_words)/sizeof(reserved_key_words[0]);

int is_valid_macro_name(const char *name, int flag) {

    for (int i = 0; i < reserved_key_words_length; i++) {
        if (strcmp(name, reserved_key_words[i]) == 0 && flag == 0) {
            printf("Invalid macro name! cannot use '%s' as a macro name!",
                reserved_key_words[i]);
            return 0;
        }
        if (strcmp(name, reserved_key_words[i]) == 0 && flag == 1) {
            return 0;
        }
    }
    return 1;
}

int is_label(const char *name) {

    int len = strlen(name);
    return len > 0 && name[len - 1] == ':';
}


char *str_dup(const char *s) {
    char *copy = malloc(strlen(s) + 1);
    if (copy != NULL) {
        strcpy(copy, s);
    }
    return copy;
}

void init_macro_list(MacroList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void add_macro(MacroList *list, const char *name) {

    if (is_valid_macro_name(name, 0) == 0) {
        exit(1);
    }

    if (list->count == list->capacity) {
        int new_capacity = list->capacity == 0 ? 4 : list->capacity * 2;
        Macro *new_items = realloc(list->items, new_capacity * sizeof(Macro));

        if (new_items == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        list->items = new_items;
        list->capacity = new_capacity;
    }

    Macro *m = &list->items[list->count++]; // Point to next free macro slot
    m->name = str_dup(name);
    m->lines = NULL;
    m->line_count = 0;
    m->capacity = 0;
}

Macro *find_macro(MacroList *list, const char *name) {

    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].name, name) == 0) {
            return  &list->items[i];
        }
    }
    return NULL;
}

void add_line_to_macro(Macro *m, const char *line) {
    if (m->line_count == m->capacity) {
        int new_capacity = m->capacity == 0 ? 4 : m->capacity * 2;
        char **new_lines = realloc(m->lines, new_capacity * sizeof(char *));

        if (new_lines == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        m->lines = new_lines;
        m->capacity = new_capacity;
    }
    m->lines[m->line_count++] = str_dup(line);
}
void free_macro(Macro *m) {
    free(m->name);

    for (int i = 0; i < m->line_count; i++) {
        free(m->lines[i]);
    }

    free(m->lines);
}

void free_macro_list(MacroList *list) {
    for (int i = 0; i < list->count; i++) {
        free_macro(&list->items[i]);
    }

    free(list->items);
}

void expand_macros(FILE *src, FILE *out, MacroList *macros)
{
    char line[256];
    int inside_macro = 0;
    Macro *current_macro = NULL;

    while (fgets(line, sizeof(line), src) != NULL) {
        char first_word[80];
        char second_word[80];

        first_word[0] = '\0';
        second_word[0] = '\0';

        sscanf(line, "%79s %79s", first_word, second_word);

        if (strcmp(first_word, "mcro") == 0) {
            add_macro(macros, second_word);
            current_macro = find_macro(macros, second_word);
            inside_macro = 1;
            continue;
        }

        if (inside_macro) {
            if (strcmp(first_word, "mcroend") == 0) {
                inside_macro = 0;
                current_macro = NULL;
                continue;
            }

            add_line_to_macro(current_macro, line);
            continue;
        }

        if (is_valid_macro_name(first_word, 1) == 0 || is_label(first_word) == 1) { // Should we even check?
            fputs(line, out);
            continue;
        }
        
        if (first_word[0] == ';' || first_word[0] == '\0') {
            fputs(line, out);
            continue;
        }

        Macro *m = find_macro(macros, first_word);

        if (m != NULL) {
            for (int i = 0; i < m->line_count; i++) {
                fputs(m->lines[i], out);
            }
        } else {
            printf("Macro : %s - not found!", first_word);
            exit(1);
        }
    }
    if (inside_macro == 1) {
        printf("Macro never ended! make sure to mark with endmcro");
        exit(1);
    }

}