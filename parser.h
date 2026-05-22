#ifndef PARSER_H
#define PARSER_H

#define MAX_LABEL_LEN 31
#define MAX_OPCODE_LEN 10
#define MAX_OPERANDS 2
#define MAX_OPERAND_LEN 31
#include "macro_helper.h"

typedef enum {
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_INSTRUCTION,
    LINE_DIRECTIVE,
    LINE_ERROR
} LineType;


typedef enum {
    DIR_INVALID = -1,
    DIR_DATA = 0,
    DIR_STRING = 1,
    DIR_ENTRY = 2,
    DIR_EXTERN = 3
} DirectiveType;

typedef struct {
    LineType type;
    int has_label;
    char label[MAX_LABEL_LEN];
    char command[MAX_OPCODE_LEN];
    char operands[MAX_OPERANDS][MAX_OPERAND_LEN];
    int operand_count;
} ParsedLine;
void trim(char *str);
int is_valid_label(const char *label, MacroList *macros);
DirectiveType is_valid_directive(const char *directive);
int parse_line(const char *line, ParsedLine *result, MacroList *macros);



#endif