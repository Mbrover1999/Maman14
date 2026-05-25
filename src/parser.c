#include <string.h>
#include "../headers/parser.h"
#include <ctype.h>
#include <stdlib.h>
#include "../headers/macro_helper.h"
#include "../headers/opcode_list.h"
#include "../headers/util.h"


/*Check if a bad register*/
int is_bad_register(const char *operand) {
    return operand[0] == 'r' &&
           operand[1] >= '0' &&
           operand[1] <= '9';
}

/*Check legal amount of commas, return 0 if legal, 1 otherwise*/
int has_bad_commas(const char *str) {
    int i = 0;

    if (str == NULL) {
        return 1;
    }

    /*Skip blank spaces or new tabs*/
    while (str[i] == ' ' || str[i] == '\t') {
        i++;
    }
    /*Illegal comma*/
    if (str[i] == ',') {
        return 1;
    }

    /*Run through the data and check for illegal commas*/
    for (; str[i] != '\0'; i++) {
        if (str[i] == ',') {
            int j = i + 1;
            /*Skip blank spaces or new tabs*/
            while (str[j] == ' ' || str[j] == '\t') {
                j++;
            }
            /*Illegal comma*/
            if (str[j] == ',' || str[j] == '\0' || str[j] == '\n') {
                return 1;
            }
        }
    }

    return 0;
}

/*Check if label is valid*/
int is_valid_label(const char *label, MacroList *macros){
    int i;
    /* check if first char is a letter*/
    if (!isalpha(label[0])) {
        printf("%c is illegal in labels!\n", label[0]);
        return 0;
    }

    /*Check the size of label, MAX_LABEL_LEN = 31*/
    if (strlen(label) >= MAX_LABEL_LEN) {
        printf("%s is too long! (max length is 31)\n", label);
        return 0;
    }

    /*Check if num or char, also eliminates .data, .extern and ect*/
    i = 0;
    for (; label[i] != '\0'; i++) {

        if (!isalnum(label[i])) {
            printf("Not the correct format!\n");
            return 0;
        }
    }

    /*Is it a command?*/
    if (find_command(label) !=NULL) {
        printf("%s is a command name! illegal!\n", label);
        return 0;
    }

    /*Is it a register?*/
    if (strcmp(label, "r0") == 0 ||
    strcmp(label, "r1") == 0 ||
    strcmp(label, "r2") == 0 ||
    strcmp(label, "r3") == 0 ||
    strcmp(label, "r4") == 0 ||
    strcmp(label, "r5") == 0 ||
    strcmp(label, "r6") == 0 ||
    strcmp(label, "r7") == 0) {
        printf("%s is a register! illegal!", label);
        return 0;
    }

    /*Is it a macro?*/
    if (find_macro(macros, label) != NULL) {
        printf("%s is a macro! illegal!", label);
        return 0;
    }

    return 1;
}

/*Check if operand is valid*/
int is_valid_operand(const char *operand, int mode, MacroList *macros) {
    /*Immediate?*/
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

    /*Relative?*/
    if (mode == ADDR_RELATIVE) {

        if (operand[1] == '\0') {
            return 0;
        }

        return is_valid_label(operand + 1, macros);
    }
    /*Direct?*/
    if (mode == ADDR_DIRECT) {

        if (is_bad_register(operand)) {
            return 0;
        }

        return is_valid_label(operand, macros);

    }

    return 0;
}

/*Return addressing mode*/
int get_addressing_mode(const char *operand) {

    /*Immediate?*/
    if (operand[0] == '#') {
        return ADDR_IMMEDIATE;
    }

    /*Relative?*/
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

/*Return directive type*/
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

/*Parse instruction's operands and return if passed inspection or not,
  1 - correct, 0 - has a problem*/
int parse_instruction_operands(char *rest, ParsedLine *result, MacroList *macros) {
    char *token = NULL;
    const CommandInfo *command = NULL;
    command = find_command(result->command);

    /*If no operands, check if it's a no operand instruction*/
    if (rest == NULL) {
        if (command->opr_num == 0) {
            return 1;
        }

        result->type = LINE_ERROR;
        return 0;
    }
    /*Check for illegal input*/
    if (has_bad_commas(rest)) {
        result->type = LINE_ERROR;
        printf("Bad commas!");
        return 0;
    }

    token = strtok(rest, ",");
    /*Cycle through the operands*/
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

    /*Operand num checks*/
    if (result->operand_count != command->opr_num) {
        result->type = LINE_ERROR;
        printf("Wrong amount of operands for %s\n", command->name);
        return 0;
    }
    if (command->opr_num == 2) {
        int src_mode = get_addressing_mode(result->operands[0]);
        int dst_mode = get_addressing_mode(result->operands[1]);

        if (!is_valid_operand(result->operands[0], src_mode, macros)) {
            printf("%s is an illegal operand!\n", result->operands[0]);
            result->type = LINE_ERROR;
            return 0;
        }

        if (!is_valid_operand(result->operands[1], dst_mode, macros)) {
            printf("%s is an illegal operand!\n", result->operands[1]);
            result->type = LINE_ERROR;
            return 0;
        }

        if (!is_addressing_legal(command->legal_src, src_mode)) {
            printf("%s is an illegal operand!\n", result->operands[0]);
            result->type = LINE_ERROR;
            return 0;
        }

        if (!is_addressing_legal(command->legal_dst, dst_mode)) {
            printf("%s is an illegal operand!\n", result->operands[1]);
            result->type = LINE_ERROR;
            return 0;
        }
    }

    /*Operand checker for sec and dect*/
    if (command->opr_num == 1) {
        int dst_mode = get_addressing_mode(result->operands[0]);

        if (!is_valid_operand(result->operands[0], dst_mode, macros)) {
            printf("%s is an illegal operand!", result->operands[0]);
            result->type = LINE_ERROR;
            return 0;
        }

        if (!is_addressing_legal(command->legal_dst, dst_mode)) {
            printf("Illegal address!\n");
            result->type = LINE_ERROR;
            return 0;
        }
    }



    return 1;
}


/*Parse direcative's operands and return if passed inspection or not,
  1 - correct, 0 - has a problem*/
int parse_directive_operands(char *rest, ParsedLine *result, MacroList *macros) {
    char *endptr = NULL;
    char *token = NULL;
    char *closing_quote = NULL;
    DirectiveType directive = is_valid_directive(result->command);

    if (rest == NULL) {
        printf("No parameters illegal!\n");
        result->type = LINE_ERROR;
        return 0;
    }

    if (directive == DIR_DATA) {
        /*Check for illegal input*/
        if (has_bad_commas(rest)) {
            printf("Bad commas!\n");
            result->type = LINE_ERROR;
            return 0;
        }


        token = strtok(rest, ",");
        /*Cycle through the operands*/
        while (token != NULL) {


            trim(token);

            /*Operand chekcs*/
            if (strlen(token) >= MAX_OPERAND_LEN) {
                printf("operand is too long\n");
                result->type = LINE_ERROR;
                return 0;
            }
            strtol(token, &endptr, 10);
            if (endptr == token || *endptr != '\0') {
                printf("Not a number illegal parameter\n");
                result->type = LINE_ERROR;
                return 0;
            }

            strcpy(result->operands[result->operand_count], token);
            result->operand_count++;
            token = strtok(NULL, ",");
        }

        /*No operands detected? error*/
        if (result->operand_count == 0) {
            printf("No valid parameters for .data!\n");
            return 0;
        }
        return 1;
    }

    if (directive == DIR_STRING) {
        char parameter[MAX_OPERAND_LEN];
        strcpy(parameter, rest);
        trim(parameter);/* trim and remove blanks from parameter*/
        /*If checks for the validity of the string parameter*/
        if (strlen(parameter) < 2) {
            result->type = LINE_ERROR;
            printf("String argument is illegal!\n");
            return 0;
        }
        if (parameter[0] != '"') {
            result->type = LINE_ERROR;
            printf("String argument is illegal missing starting quotes!\n");
            return 0;
        }

        closing_quote = strchr(parameter + 1, '"');

        if (closing_quote == NULL) {
            result->type = LINE_ERROR;
            printf("String argument is illegal missing ending quotes!\n");
            return 0;
        }

        closing_quote++;

        while (*closing_quote == ' ' || *closing_quote == '\t') {
            closing_quote++;
        }

        if (*closing_quote != '\0') {
            result->type = LINE_ERROR;
            printf("String argument is illegal!\n");
            return 0;
        }

        if (parameter[strlen(parameter) - 1] != '"') {
            printf("String argument is illegal!\n");
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
            printf("Entry argument is illegal!\n");
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
        trim(parameter); /*trim and remove blanks from parameter*/
        /*Check if extern parameter name is legal*/
        if (!is_valid_label(parameter, macros)) {
            printf("Extern argument is illegal!\n");
            result->type = LINE_ERROR;
            return 0;
        }

        strcpy(result->operands[0], parameter);
        result->operand_count = 1;

        return 1;
    }
    return 0;
    }


/*Main method, take in a line from first pass/ second pass, 0 - failure, 1 - success*/
int parse_line(const char *line, ParsedLine *result, MacroList *macros) {

    char buffer[256];
    char *ptr = NULL;
    char *token = NULL;
    char *colon = NULL;

    strcpy(buffer, line);

    ptr = buffer;

    /*skip spaces*/
    while (*ptr == ' ' || *ptr == '\t'|| *ptr == '\n' ) {
        ptr++;
    }

    /*empty/comment*/
    if (*ptr == '\0' || *ptr == ';') {
        result->type = LINE_COMMENT;
        return 1;
    }

    result->has_label = 0;
    result->operand_count = 0;

    /*Get label name*/
    colon = strchr(ptr, ':');

    if (colon != NULL) {

        /*Get the size of label to copy*/
        int len = (int)(colon - ptr);

        if (len <= 0 || len >= MAX_LABEL_LEN) {
            printf("Label length is illegal!\n");
            result->type = LINE_ERROR;
            return 0;
        }

        strncpy(result->label, ptr, len);
        result->label[len] = '\0';
        trim(result->label);
        /*Agin check if a valid label*/
        if (!is_valid_label(result->label, macros)) {
            printf("Label '%s' is illegal!\n", result->label);
            result->type = LINE_ERROR;
            return 0;
        }

        result->has_label = 1;

        ptr = colon + 1;
    }

    while (*ptr == ' ' || *ptr == '\t') {
        ptr++;
    }

    /*command*/
    token = strtok(ptr, " \t\n");

    if (token == NULL) {
        result->type = LINE_ERROR;
        printf("Missing command!\n");
        return 0;
    }

    strcpy(result->command, token);

    /*directive or instruction*/
    if (result->command[0] == '.') {
        /*Check if directive is valid*/
        if (is_valid_directive(result->command) == DIR_INVALID) {
            printf("'%s' is an Illegal command!\n", result->command);
            result->type = LINE_ERROR;
            return 0;
        }

        result->type = LINE_DIRECTIVE;

    }
    else {
        /*Check if command is a legal command*/
        if (find_command(result->command) == NULL) {
            printf("'%s' is an Illegal command!\n", result->command);
            result->type = LINE_ERROR;
            return 0;
        }
        result->type = LINE_INSTRUCTION;
    }

    /*operands*/
    token = strtok(NULL, "\n");

    if (result->type == LINE_INSTRUCTION) {
        return parse_instruction_operands(token, result, macros);
    }

    return parse_directive_operands(token, result, macros);


}