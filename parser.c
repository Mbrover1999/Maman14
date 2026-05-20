#include <string.h>
#include <stdio.h>

#include "parser.h"

#include <ctype.h>

#include "macro_helper.h"
#include "opcode_list.h"

void trim(char *str) {

    char *start = str;
    char *end = NULL;

    /* skip spaces at start */
    while (*start == ' ' || *start == '\t' || *start == '\n') {
        start++;
    }

    memmove(str, start, strlen(start) + 1);

    if (*str == '\0') {
        return;
    }


    /* find end */
    end = str + strlen(str) - 1;

    /* remove spaces at end */
    while (end >= str &&
          (*end == ' ' || *end == '\t' || *end == '\n')) {

        *end = '\0';
        end--;
          }
}

int is_valid_label(const char *label, MacroList *macros) {

    if (!isalpha(label[0])) {
        return 0;
    }
    if (strlen(label) >= MAX_LABEL_LEN) {
        return 0;
    }

    int i = 0;
    for (; label[i] != '\0'; i++) {

        if (!isalnum(label[i])) {
            return 0;
        }
    }

    if (find_command(label) !=NULL) {
        return 0;
    }

    if (strcmp(label, "r0") == 0 ||
    strcmp(label, "r1") == 0 ||
    strcmp(label, "r2") == 0 ||
    strcmp(label, "r3") == 0 ||
    strcmp(label, "r4") == 0 ||
    strcmp(label, "r5") == 0 ||
    strcmp(label, "r6") == 0 ||
    strcmp(label, "r7") == 0) {

        return 0;
    }

    if (find_macro(macros, label) != NULL) {
        return 0;
    }

    return 1;
}

int parse_line(const char *line, ParsedLine *result, MacroList *macros) {

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

        if (len <= 0 || len >= MAX_LABEL_LEN) {
            result->type = LINE_ERROR;
            return 0;
        }

        strncpy(result->label, ptr, len);
        result->label[len] = '\0';
        trim(result->label);

        if (!is_valid_label(result->label, macros)) {
            result->type = LINE_ERROR;
            return 0;
        }

        result->has_label = 1;

        ptr = colon + 1;
    }

    while (*ptr == ' ' || *ptr == '\t') {
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
    token = strtok(NULL, "\n");

    if (token == NULL) {
        return 1;
    }
    token = strtok(token, ",");

    while (token != NULL && result->operand_count < MAX_OPERANDS) {

        while (*token == ' ' || *token == '\t') {
            token++;
        }
        if (strlen(token) >= MAX_OPERAND_LEN) {
            result->type = LINE_ERROR;
            return 0;
        }
        char operand[MAX_OPERAND_LEN];
        strcpy(operand, token);
        trim(operand);

        strcpy(result->operands[result->operand_count], operand);

        result->operand_count++;

        token = strtok(NULL, ",");
    }

    return 1;
}