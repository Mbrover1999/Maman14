#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>

typedef struct {
    char *name;
    char **lines;
    int line_count;
    int capacity;
} Macro;

typedef struct {
    Macro *items;
    int count;
    int capacity;
} MacroList;

void init_macro_list(MacroList *list);
void add_macro(MacroList *list, const char *name);
Macro *find_macro(MacroList *list, const char *name);
void add_line_to_macro(Macro *m, const char *line);
void free_macro_list(MacroList *list);

void expand_macros(FILE *src, FILE *out, MacroList *macros);

#endif