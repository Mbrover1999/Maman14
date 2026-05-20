#ifndef PARSER_H
#define PARSER_H

#define MAX_LABEL_LEN 31
#define MAX_OPCODE_LEN 10
#define MAX_OPERANDS 2
#define MAX_OPERAND_LEN 31

typedef enum {
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_INSTRUCTION,
    LINE_DIRECTIVE,
    LINE_ERROR
} LineType;

typedef struct {
    LineType type;

    int has_label;
    char label[MAX_LABEL_LEN];

    char command[MAX_OPCODE_LEN];

    char operands[MAX_OPERANDS][MAX_OPERAND_LEN];
    int operand_count;
} ParsedLine;

int parse_line(const char *line, ParsedLine *result);

#endif