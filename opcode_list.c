#include <string.h>
#include "opcode_list.h"

static CommandInfo commands[] = {
    {"mov",  0, 2},
    {"cmp",  1, 2},
    {"add",  2, 2},
    {"sub",  3, 2},
    {"lea",  4, 2},

    {"clr",  5, 1},
    {"not",  6, 1},
    {"inc",  7, 1},
    {"dec",  8, 1},
    {"jmp",  9, 1},
    {"bne", 10, 1},
    {"red", 11, 1},
    {"prn", 12, 1},
    {"jsr", 13, 1},

    {"rts", 14, 0},
    {"stop", 15, 0}
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