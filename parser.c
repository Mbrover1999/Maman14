#include <string.h>
#include "helpers/parser.h"
#include <ctype.h>
#include <stdlib.h>
#include "helpers/macro_helper.h"
#include "helpers/opcode_list.h"

void trim(char *str) {

    char *start = str;
    char *end = NULL;

    // skip spaces at start
    while (*start == ' ' || *start == '\t' || *start == '\n') {
        start++;
    }

    memmove(str, start, strlen(start) + 1);

    if (*str == '\0') {
        return;
    }


    // find end
    end = str + strlen(str) - 1;

    /* remove spaces at end */
    while (end >= str &&
          (*end == ' ' || *end == '\t' || *end == '\n')) {

        *end = '\0';
        end--;
          }
}
int has_bad_commas(const char *str) {
    int i = 0;

    if (str == NULL) {
        return 1;
    }

    while (str[i] == ' ' || str[i] == '\t') {
        i++;
    }

    if (str[i] == ',') {
        return 1;
    }

    for (; str[i] != '\0'; i++) {
        if (str[i] == ',') {
            int j = i + 1;

            while (str[j] == ' ' || str[j] == '\t') {
                j++;
            }

            if (str[j] == ',' || str[j] == '\0' || str[j] == '\n') {
                return 1;
            }
        }
    }

    return 0;
}
int is_valid_label(const char *label, MacroList *macros){
    // check if first char is a letter
    if (!isalpha(label[0])) {
        return 0;
    }

    // check the size of label, MAX_LABEL_LEN = 31
    if (strlen(label) >= MAX_LABEL_LEN) {
        return 0;
    }

    // check if num or char, also eliminates .data, .extern and ect
    int i = 0;
    for (; label[i] != '\0'; i++) {

        if (!isalnum(label[i])) {
            return 0;
        }
    }

    // is it a command?
    if (find_command(label) !=NULL) {
        return 0;
    }

    // is it a register?
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

    // is it a macro?
    if (find_macro(macros, label) != NULL) {
        return 0;
    }

    return 1;
}
int is_valid_operand(const char *operand, int mode, MacroList *macros) {

    if (mode == ADDR_IMMEDIATE) {

        char *endptr = NULL;

        if (operand[1] == '\0') {
            return 0;
        }

        strtol(operand + 1, &endptr, 10);

        return (*endptr == '\0');
    }

    if (mode == ADDR_REGISTER) {

        return (
            operand[0] == 'r' &&
            operand[1] >= '0' &&
            operand[1] <= '7' &&
            operand[2] == '\0'
        );
    }

    if (mode == ADDR_RELATIVE) {

        if (operand[1] == '\0') {
            return 0;
        }

        return is_valid_label(operand + 1, macros);
    }

    if (mode == ADDR_DIRECT) {

        return is_valid_label(operand, macros);
    }

    return 0;
}
int get_addressing_mode(const char *operand) {

    if (operand[0] == '#') {
        return ADDR_IMMEDIATE;
    }

    if (operand[0] == '%') {
        return ADDR_RELATIVE;
    }

    if (operand[0] == 'r' &&
        operand[1] >= '0' &&
        operand[1] <= '7' &&
        operand[2] == '\0') {

        return ADDR_REGISTER;
        }

    return ADDR_DIRECT;
}

DirectiveType is_valid_directive(const char *directive) {

    if (strcmp(directive, ".data") == 0) {
        return DIR_DATA;
    }

    if (strcmp(directive, ".string") == 0) {
        return DIR_STRING;
    }

    if (strcmp(directive, ".entry") == 0) {
        return DIR_ENTRY;
    }

    if (strcmp(directive, ".extern") == 0) {
        return DIR_EXTERN;
    }

    return DIR_INVALID;
}

int parse_instruction_operands(char *rest, ParsedLine *result, MacroList *macros) {
    char *token = NULL;
    const CommandInfo *command = NULL;
    command = find_command(result->command);

    if (rest == NULL) {
        if (command->opr_num == 0) {
            return 1;
        }

        result->type = LINE_ERROR;
        return 0;
    }

    if (has_bad_commas(rest)) {
        result->type = LINE_ERROR;
        return 0;
    }

    token = strtok(rest, ",");

    while (token != NULL && result->operand_count < MAX_OPERANDS) {
        char operand[MAX_OPERAND_LEN];

        trim(token);

        if (strlen(token) >= MAX_OPERAND_LEN) {
            result->type = LINE_ERROR;
            return 0;
        }

        strcpy(operand, token);
        strcpy(result->operands[result->operand_count], operand);

        result->operand_count++;
        token = strtok(NULL, ",");
    }

    if (token != NULL) {
        result->type = LINE_ERROR;
        return 0;
    }

    if (result->operand_count != command->opr_num) {
        result->type = LINE_ERROR;
        printf("Wrong amount of operands for %s\n", command->name);
        return 0;
    }
    if (command->opr_num == 2) {
        int src_mode = get_addressing_mode(result->operands[0]);
        int dst_mode = get_addressing_mode(result->operands[1]);

        if (!is_valid_operand(result->operands[0], src_mode, macros)) {
            result->type = LINE_ERROR;
            return 0;
        }

        if (!is_valid_operand(result->operands[1], dst_mode, macros)) {
            result->type = LINE_ERROR;
            return 0;
        }

        if (!is_addressing_legal(command->legal_src, src_mode)) {
            result->type = LINE_ERROR;
            return 0;
        }

        if (!is_addressing_legal(command->legal_dst, dst_mode)) {
            result->type = LINE_ERROR;
            return 0;
        }
    }

    if (command->opr_num == 1) {
        int dst_mode = get_addressing_mode(result->operands[0]);

        if (!is_valid_operand(result->operands[0], dst_mode, macros)) {
            result->type = LINE_ERROR;
            return 0;
        }

        if (!is_addressing_legal(command->legal_dst, dst_mode)) {
            result->type = LINE_ERROR;
            return 0;
        }
    }



    return 1;
}

int parse_directive_operands(char *rest, ParsedLine *result, MacroList *macros) {

    char *token = NULL;
    DirectiveType directive = is_valid_directive(result->command);

    if (rest == NULL) {
        printf("No parameters illegal!");
        result->type = LINE_ERROR;
        return 0;
    }

    if (directive == DIR_DATA) {

        if (has_bad_commas(rest)) {
            result->type = LINE_ERROR;
            return 0;
        }


        token = strtok(rest, ",");

        while (token != NULL) {


            trim(token);

            if (strlen(token) >= MAX_OPERAND_LEN) {
                result->type = LINE_ERROR;
                return 0;
            }
            char *endptr = NULL;
            strtol(token, &endptr, 10);
            if (endptr == token || *endptr != '\0') {
                printf("Not a number illegal parameter");
                result->type = LINE_ERROR;
                return 0;
            }

            strcpy(result->operands[result->operand_count], token);
            result->operand_count++;
            token = strtok(NULL, ",");
        }

        if (result->operand_count <= 0) {
            printf("No valid parameters for .data!");
            return 0;
        }
        return 1;
    }
    if (directive == DIR_STRING) {
        char parameter[MAX_OPERAND_LEN];
        strcpy(parameter, rest);
        trim(parameter);

        if (strlen(parameter) < 2) {
            result->type = LINE_ERROR;
            return 0;
        }
        if (parameter[0] != '"') {
            result->type = LINE_ERROR;
            return 0;
        }

        char *closing_quote = strchr(parameter + 1, '"');

        if (closing_quote == NULL) {
            result->type = LINE_ERROR;
            return 0;
        }

        closing_quote++;

        while (*closing_quote == ' ' || *closing_quote == '\t') {
            closing_quote++;
        }

        if (*closing_quote != '\0') {
            result->type = LINE_ERROR;
            return 0;
        }

        if (parameter[strlen(parameter) - 1] != '"') {
            result->type = LINE_ERROR;
            return 0;
        }

        strcpy(result->operands[0], parameter);
        result->operand_count = 1;

        return 1;
    }

    if (directive == DIR_ENTRY) {
        char parameter[MAX_OPERAND_LEN];
        strcpy(parameter, rest);
        trim(parameter);

        if (!is_valid_label(parameter, macros)) {
            result->type = LINE_ERROR;
            return 0;
        }

        strcpy(result->operands[0], parameter);
        result->operand_count = 1;

        return 1;
    }
    if (directive == DIR_EXTERN) {
        char parameter[MAX_OPERAND_LEN];
        strcpy(parameter, rest);
        trim(parameter);

        if (!is_valid_label(parameter, macros)) {
            result->type = LINE_ERROR;
            return 0;
        }

        strcpy(result->operands[0], parameter);
        result->operand_count = 1;

        return 1;
    }
    return 0;
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

    // handle label
    colon = strchr(ptr, ':');

    if (colon != NULL) {

        // get the size of label to copy
        int len = (int)(colon - ptr);

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

        if (is_valid_directive(result->command) == DIR_INVALID) {
            result->type = LINE_ERROR;
            return 0;
        }

        result->type = LINE_DIRECTIVE;

    }
    else {
        if (find_command(result->command) == NULL) {
            result->type = LINE_ERROR;
            return 0;
        }
        result->type = LINE_INSTRUCTION;
    }

    /* operands */
    token = strtok(NULL, "\n");

    if (result->type == LINE_INSTRUCTION) {
        return parse_instruction_operands(token, result, macros);
    }

    return parse_directive_operands(token, result, macros);


}