#include <string.h>
#include <stdio.h>

#include "parser.h"

int parse_line(const char *line, ParsedLine *result) {

    char buffer[256];
    char *ptr;
    char *token;
    char *colon;

    strcpy(buffer, line);

    ptr = buffer;

    /* skip spaces */
    while (*ptr == ' ' || *ptr == '\t'|| *ptr == '\n' ) {
        ptr++;
    }

    /* empty/comment */
    if (*ptr == '\0' || *ptr == ';') {
        result->type = LINE_COMMENT;
        return 1;
    }

    result->has_label = 0;
    result->operand_count = 0;

    /* label */
    colon = strchr(ptr, ':');

    if (colon != NULL) {

        // get the size of label to copy
        int len = colon - ptr;

        strncpy(result->label, ptr, len);
        result->label[len] = '\0';

        result->has_label = 1;

        ptr = colon + 1;
    }

    while (*ptr == ' ') {
        ptr++;
    }

    /* command */
    token = strtok(ptr, " \t\n");

    if (token == NULL) {
        result->type = LINE_ERROR;
        return 0;
    }

    strcpy(result->command, token);

    /* directive or instruction */
    if (result->command[0] == '.') {
        result->type = LINE_DIRECTIVE;
    } else {
        result->type = LINE_INSTRUCTION;
    }

    /* operands */
    token = strtok(NULL, ",");

    while (token != NULL && result->operand_count < MAX_OPERANDS) {

        while (*token == ' ') {
            token++;
        }

        strcpy(result->operands[result->operand_count], token);

        result->operand_count++;

        token = strtok(NULL, ",");
    }

    return 1;
}