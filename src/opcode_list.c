#include <string.h>
#include "../headers/opcode_list.h"

static CommandInfo commands[] = {

    /* two operands */

    {"mov", 0, -1, 2,
        ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER,
        ADDR_DIRECT | ADDR_REGISTER},

    {"cmp", 1, -1, 2,
        ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER,
        ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER},

    {"add", 2, 10, 2,
        ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER,
        ADDR_DIRECT | ADDR_REGISTER},

    {"sub", 2, 11, 2,
        ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER,
        ADDR_DIRECT | ADDR_REGISTER},

    {"lea", 4, -1, 2,
        ADDR_DIRECT,
        ADDR_DIRECT | ADDR_REGISTER},

    /* one operand */

    {"clr", 5, 10, 1,
        ADDR_NONE,
        ADDR_DIRECT | ADDR_REGISTER},

    {"not", 5, 11, 1,
        ADDR_NONE,
        ADDR_DIRECT | ADDR_REGISTER},

    {"inc", 5, 12, 1,
        ADDR_NONE,
        ADDR_DIRECT | ADDR_REGISTER},

    {"dec", 5, 13, 1,
        ADDR_NONE,
        ADDR_DIRECT | ADDR_REGISTER},

    {"jmp", 9, 10, 1,
        ADDR_NONE,
        ADDR_DIRECT | ADDR_RELATIVE},

    {"bne", 9, 11, 1,
        ADDR_NONE,
        ADDR_DIRECT | ADDR_RELATIVE},

    {"jsr", 9, 12, 1,
        ADDR_NONE,
        ADDR_DIRECT | ADDR_RELATIVE},

    {"red", 12, -1, 1,
        ADDR_NONE,
        ADDR_DIRECT | ADDR_REGISTER},

    {"prn", 13, -1, 1,
        ADDR_NONE,
        ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER},

    /* no operands */

    {"rts", 14, -1, 0,
        ADDR_NONE,
        ADDR_NONE},

    {"stop", 15, -1, 0,
        ADDR_NONE,
        ADDR_NONE}
};

#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

CommandInfo *find_command(const char *name) {
    int i = 0;

    for (; i < COMMAND_COUNT; i++) {
        if (strcmp(commands[i].name, name) == 0) {
            return &commands[i];
        }
    }



    return NULL;
}
int is_addressing_legal(int legal_modes, int mode) {
    return (legal_modes & mode) != 0;
}