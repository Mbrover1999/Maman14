#include <stdio.h>
#include <stdlib.h>

#include "assembler_table.h"
#include "mem_img.h"
#include "opcode_list.h"
#include "parser.h"


int encode_second_pass(const char *operand, AssemblerTable *table,
    int current_address,int *ARE, int *error_flag, int line_num) {
    int value;
    int mode = get_addressing_mode(operand);

    if (mode == ADDR_IMMEDIATE) {
        *ARE = 'A';
        value =  (int)strtol(operand + 1, NULL, 10);
        return value;
    }

    if (mode == ADDR_REGISTER) {
        *ARE = 'A';
        value =  operand[1] - '0';
        return 1 << value;
    }

    if (mode == ADDR_DIRECT) {
        Symbol *symbol = find_symbol(table, operand);

        if (symbol == NULL) {
            printf("Cant find symbol in line: %d\n", line_num);
            *error_flag = 1;
            return 0;
        }

        if (symbol->attributes & ATTR_EXTERN) {
            *ARE = 'E';
            return 0;
        }

        *ARE = 'R';
        return symbol->value;
    }
    if (mode == ADDR_RELATIVE) {
        Symbol *symbol = find_symbol(table, operand + 1);

        if (symbol == NULL) {
            printf("Cant find symbol in line: %d\n", line_num);
            *error_flag = 1;
            return 0;
        }

        *ARE = 'A';
        value =  symbol->value - current_address;
        return value;
    }

    *ARE = '?';
    return 0;
}

void second_pass(FILE *in,AssemblerTable *table,MacroList *macros,
    MemoryImage *code_image,int *error_flag) {
    char line[256];
    ParsedLine parsed;
    int IC = 100;
    int line_num = 0;

    rewind(in);

    while (fgets(line, sizeof(line), in) != NULL) {
        line_num++;

        if (!parse_line(line, &parsed, macros)) {
            printf("Second pass parse error in line: %d\n", line_num);
            *error_flag = 1;
            continue;
        }

        if (parsed.type == LINE_COMMENT || parsed.type == LINE_ERROR) {
            continue;
        }

        if (parsed.type == LINE_DIRECTIVE) {
            DirectiveType directive = is_valid_directive(parsed.command);

            if (directive == DIR_ENTRY) {
                Symbol *symbol = find_symbol(table, parsed.operands[0]);
                if (symbol == NULL) {
                    printf("Entry symbol not found: %s in line num: %d\n",
                        parsed.operands[0], line_num);
                        *error_flag = 1;
                    continue;
                }

                if (symbol->attributes & ATTR_EXTERN) {
                    printf("Symbol cannot be both entry and extern: %s in line %d\n",
                        parsed.operands[0], line_num);
                    *error_flag = 1;
                    continue;
                }
                symbol->attributes |= ATTR_ENTRY;
            }

            continue;
        }

        if (parsed.type == LINE_INSTRUCTION) {
            int i = 0;
            int operand_address = IC + 1;

            for (; i < parsed.operand_count; i++) {
                int ARE;
                int value;
                MemoryWord *word = NULL;

                word = find_by_address(code_image, operand_address);

                if (word == NULL) {
                    printf("Missing code word at address %d\n", operand_address);
                    *error_flag = 1;
                    break;
                    }

                value = encode_second_pass(parsed.operands[i],table,operand_address,&ARE,
                   error_flag, line_num);

                word->value = value;

                operand_address++;

                }

            IC += 1 + parsed.operand_count;
            }
        }
    }

