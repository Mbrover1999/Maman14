#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H
#define ADDR_IMMEDIATE 1
#define ADDR_DIRECT    2
#define ADDR_RELATIVE  4
#define ADDR_REGISTER  8
#define ADDR_NONE      0


typedef enum {
    TWO_OPERANDS,
    ONE_OPERAND,
    NO_OPERANDS
} OperandNum;


typedef struct {
    char *name;
    int opcode;
    int funct;
    OperandNum opr_num;
    int legal_src;
    int legal_dst;
} CommandInfo;

CommandInfo *find_command(const char *name);
int is_addressing_legal(int legal_modes, int mode);

#endif