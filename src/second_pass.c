#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/assembler_table.h"
#include "../headers/mem_img.h"
#include "../headers/opcode_list.h"
#include "../headers/parser.h"


/*Second pass encoder here we finish the job created in the first pass,
  update to the correct ARE value('?' -> 'A,R,E')*/
int encode_second_pass(const char *operand, AssemblerTable *table,
                       int current_address,char *ARE, int *error_flag, int line_num
                       ,MemoryWord *word) {
    int value;
    int mode = get_addressing_mode(operand);
    /*Immediate?*/
    if (mode == ADDR_IMMEDIATE) {
        *ARE = 'A';
        value =  (int)strtol(operand + 1, NULL, 10);
        return value;
    }
    /*Register?*/
    if (mode == ADDR_REGISTER) {
        *ARE = 'A';
        value =  operand[1] - '0';
        return 1 << value;
    }
    /*Direct?*/
    if (mode == ADDR_DIRECT) {
        Symbol *symbol = find_symbol(table, operand);

        /*Check if exists*/
        if (symbol == NULL) {
            printf("Cant find symbol '%s' in line: %d\n",(operand), line_num);
            *error_flag = 1;
            return 0;
        }

        if (symbol->attributes & ATTR_EXTERN) {
            *ARE = 'E';
            strcpy(word->symbol_name, symbol->name);
            return 0;
        }

        *ARE = 'R';
        return symbol->value;
    }
    /*Relative?*/
    if (mode == ADDR_RELATIVE) {
        Symbol *symbol = find_symbol(table, operand + 1);

        /*Check if exists*/
        if (symbol == NULL) {
            printf("Cant find symbol '%s' in line: %d\n",(operand+1), line_num);
            *error_flag = 1;
            return 0;
        }

        *ARE = 'A';
        value =  symbol->value - current_address;
        return value;
    }
    /*If for any reason something went horribly wrong keep the '?'*/
    *ARE = '?';
    return 0;
}


/*Main method in second pass, move through the file again and fill missing data*/
void second_pass(FILE *in,AssemblerTable *table,MacroList *macros,
    MemoryImage *code_image,int *error_flag) {
    char line[256];
    ParsedLine parsed; /*Holds the parsed code line*/
    int IC = 100;
    int line_num = 0; /*Current line num*/

    rewind(in);

    while (fgets(line, sizeof(line), in) != NULL) {
        line_num++;
        /*Parse failure*/
        if (!parse_line(line, &parsed, macros)) {
            printf("Second pass parse error in line: %d\n", line_num);
            *error_flag = 1;
            continue;
        }
        /*Skip command or error lines*/
        if (parsed.type == LINE_COMMENT || parsed.type == LINE_ERROR) {
            continue;
        }
        /*Directive?*/
        if (parsed.type == LINE_DIRECTIVE) {
            DirectiveType directive = is_valid_directive(parsed.command);
            /*Entry?*/
            if (directive == DIR_ENTRY) {
                Symbol *symbol = find_symbol(table, parsed.operands[0]);
                /*Does it exist?*/
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
        /*Instruction?*/
        if (parsed.type == LINE_INSTRUCTION) {
            int i = 0;
            int operand_address = IC + 1;
            /*Move, encode and update operands */
            for (; i < parsed.operand_count; i++) {
                char ARE;
                int value;
                MemoryWord *word = NULL;

                word = find_by_address(code_image, operand_address);
                /*Missing in memory*/
                if (word == NULL) {
                    printf("Missing code word at address %d\n", operand_address);
                    *error_flag = 1;
                    break;
                    }

                value = encode_second_pass(parsed.operands[i],table,operand_address,&ARE,
                   error_flag, line_num, word);

                word->value = value;
                word->ARE = ARE;

                operand_address++;

                }

            IC += 1 + parsed.operand_count;
            }
        }
    }

