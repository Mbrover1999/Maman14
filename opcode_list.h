#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H

typedef struct {
    char *name;
    int opcode;
    int operand_count;
} CommandInfo;

CommandInfo *find_command(const char *name);

#endif