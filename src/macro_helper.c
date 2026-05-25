#include <stdlib.h>
#include <string.h>
#include "../headers/macro_helper.h"
#include "../headers/util.h"



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

/*Get the size of our keywords array*/
const int reserved_key_words_length =
    sizeof(reserved_key_words)/sizeof(reserved_key_words[0]);
/*Validate the macro name(I know it wasn't part of the maman but still
  flag, is to determine if we need to check a macro or just to check a keyword*/
int is_valid_macro_name(const char *name, int flag) {

    int i = 0;

    /*Check if the macro is a keyword*/
    for (; i < reserved_key_words_length; i++) {
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


/*Check if a valid label, used in many stages of the assembler*/
int is_label(const char *name) {

    int len = strlen(name);
    return len > 0 && name[len - 1] == ':';
}


/*Initiate the macro list*/
void init_macro_list(MacroList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

/*Add a macro and check for errors*/
void add_macro(MacroList *list, const char *name) {
    Macro *m = NULL;
    /*Is it valid?*/
    if (is_valid_macro_name(name, 0) == 0) {
        printf("illegal macro!\n");
    }
    /*Valid*/
    if (list->count == list->capacity) {
        /*Re allocating space for our macro list*/
        int new_capacity = list->capacity == 0 ? 4 : list->capacity * 2;
        Macro *new_items = realloc(list->items, new_capacity * sizeof(Macro));
        /*Failed allocation*/
        if (new_items == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        list->items = new_items;
        list->capacity = new_capacity;
    }

    m = &list->items[list->count++]; /* Point to next free macro slot*/
    m->name = str_dup(name);
    m->lines = NULL;
    m->line_count = 0;
    m->capacity = 0;
}

/*Find a macro, if failed return NULL*/
Macro *find_macro(MacroList *list, const char *name) {
    int i;
    i = 0;
    for (; i < list->count; i++) {
        if (strcmp(list->items[i].name, name) == 0) {
            return  &list->items[i];
        }
    }
    return NULL;
}


/*Add the code line to the macro*/
void add_line_to_macro(Macro *m, const char *line) {
    if (m->line_count == m->capacity) {
        /*Re allocating space for our macro's code list*/
        int new_capacity = m->capacity == 0 ? 4 : m->capacity * 2;
        char **new_lines = realloc(m->lines, new_capacity * sizeof(char *));
        /*Failed allocation*/
        if (new_lines == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        m->lines = new_lines;
        m->capacity = new_capacity;
    }
    m->lines[m->line_count++] = str_dup(line); /*Point to next free macro code slot*/
}

/*Free memory*/
void free_macro(Macro *m) {
    int i;
    free(m->name);

    i = 0;
    for (; i < m->line_count; i++) {
        free(m->lines[i]);
    }

    free(m->lines);
}
/*Free memory*/
void free_macro_list(MacroList *list) {

    int i = 0;

    for (; i < list->count; i++) {
        free_macro(&list->items[i]);
    }

    free(list->items);
}

/*Main function of macro helper, start expanding the macros line by line*/
int expand_macros(FILE *src, FILE *out, MacroList *macros)
{
    char line[256];
    int i;
    int inside_macro = 0; /*Are we inside a macro?*/
    Macro *current_macro = NULL;

    while (fgets(line, sizeof(line), src) != NULL) {
        Macro *m = NULL;
        char first_word[80]; /*holds the first portion of a code line*/
        char second_word[80]; /*holds the second portion of a code line*/

        first_word[0] = '\0';
        second_word[0] = '\0';


        /*Read first word ignore blank spaces, tabs or new line,
         then the same for the second word*/
        sscanf(line, "%79s %79s", first_word, second_word);
        /*Check if it's a macro creation*/
        if (strcmp(first_word, "mcro") == 0) {
            add_macro(macros, second_word);
            current_macro = find_macro(macros, second_word);
            inside_macro = 1;
            continue;
        }
        /*Are we still inside a macro if so add the lines to the macro, end with mcroend*/
        if (inside_macro) {
            if (strcmp(first_word, "mcroend") == 0) {
                inside_macro = 0;
                current_macro = NULL;
                continue;
            }

            add_line_to_macro(current_macro, line);
            continue;
        }

        if (is_valid_macro_name(first_word, 1) == 0 || is_label(first_word) == 1) { /* Should we even check?*/
            fputs(line, out);
            continue;
        }
        /*A comment or a blank line? add to the file and move on*/
        if (first_word[0] == ';' || first_word[0] == '\0') {
            fputs(line, out);
            continue;
        }

        /*Find the macro*/
        m = find_macro(macros, first_word);

        /*A macro? if so add the lines inside the macro replacing the macro,
          else add the line as is*/
        if (m != NULL) {
            i = 0;
            for (; i < m->line_count; i++) {
                fputs(m->lines[i], out);
            }
        } else {
           fputs(line, out);

        }
    }
    if (inside_macro == 1) {
        printf("Macro never ended! make sure to mark with endmcro\n");
        return 0;
    }
    return 1;
}